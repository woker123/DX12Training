#include "LightApp.h"
#include "d3dx12.h"
#include "GeometryGenerator.h"
#include <d3dcompiler.h>
#include "D3DUtil.h"
#include "LightFrameResource.h"
#include <string>

LightApp::~LightApp()
{
	FlushCommandQueue();
}

bool LightApp::InitializeApp(HINSTANCE hInstance)
{
	if (!D3DApp::InitializeApp(hInstance))
		return false;

	mCmdAllacator->Reset();
	mCmdList->Reset(mCmdAllacator.Get(), nullptr);

	if (!BuildRootSignature())
		return false;

	if (!BuildMeshGeometry())
		return false;

	if (!BuildLights())
		return false;

	if (!BuildShaders())
		return false;

	if (!BuildPSO())
		return false;

	if (!BuildRenderItems())
		return false;

	if (!BuildFrameResources())
		return false;
	
	if (!InitCamera())
		return false;

	mCmdList->Close();
	mCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList*const*)mCmdList.GetAddressOf());
	FlushCommandQueue();

	return true;
}

bool LightApp::BuildFrameResources()
{
	size_t numLights = mDirectionalLights.size() + mPointLights.size() + mSpotLights.size();
	for (int i = 0; i < mNumFrameResources; ++i)
	{
		mFrameResources.push_back(LightFrameResource(mDevice.Get(), 1, (UINT)mOpaqueRenderItems.size()));
	}

	return true;
}

bool LightApp::BuildMeshGeometry()
{
	//generate mesh data
	auto boxMesh = GeometryGenerator::GenerateBox(60.f, 4.f, 50.f);
	auto cylinderMesh = GeometryGenerator::GenerateCylinder(2.f, 1.f, 20.f, 32, 32);
	auto sphereMesh = GeometryGenerator::GenerateSphere(1.f, 32, 32);
	const int meshNum = 3;

	//a bunch of memory for combine all geometry mesh data
	std::vector<Vertex> vertices(boxMesh.Vertices.size() + cylinderMesh.Vertices.size() + sphereMesh.Vertices.size());
	std::vector<uint32> indices(boxMesh.Indices32.size() + cylinderMesh.Indices32.size() + sphereMesh.Indices32.size());
	MeshData* meshes[meshNum] = {&boxMesh, &cylinderMesh, &sphereMesh};
	UINT startVertex = 0, startIndex = 0;
	for (int i = 0; i < meshNum; ++i)
	{
		for (int j = 0; j < meshes[i]->Vertices.size(); ++j)
			vertices[(UINT64)startVertex + j] = meshes[i]->Vertices[j];

		for (int j = 0; j < meshes[i]->Indices32.size(); ++j)
			indices[(UINT64)startIndex + j] = meshes[i]->Indices32[j];

		startVertex += (UINT)meshes[i]->Vertices.size();
		startIndex += (UINT)meshes[i]->Indices32.size();
	}

	//vertex offset
	UINT boxMeshVertexOffset = 0;
	UINT cylinderMeshVertexOffset = (UINT)boxMesh.Vertices.size();
	UINT sphereMeshVertexOffset = cylinderMeshVertexOffset + (UINT)cylinderMesh.Vertices.size();

	//index offset
	UINT boxMeshIndexOffset = 0;
	UINT cylinderMeshIndexOffset = (UINT)boxMesh.Indices32.size();
	UINT sphereMeshIndexOffset = cylinderMeshIndexOffset + (UINT)cylinderMesh.Indices32.size();

	/**sub geometies*/
	//box mesh
	SubMeshGeometry boxSubGeo = {};
	boxSubGeo.baseVertexLocation = boxMeshVertexOffset;
	boxSubGeo.indexCount = (UINT)boxMesh.Indices32.size();
	boxSubGeo.startIndexLocation = boxMeshIndexOffset;

	//cylinder mesh
	SubMeshGeometry cylinderSubGeo = {};
	cylinderSubGeo.baseVertexLocation = cylinderMeshVertexOffset;
	cylinderSubGeo.indexCount = (UINT)cylinderMesh.Indices32.size();
	cylinderSubGeo.startIndexLocation = cylinderMeshIndexOffset;

	//shpere mesh
	SubMeshGeometry sphereSubGeo = {};
	sphereSubGeo.baseVertexLocation = sphereMeshVertexOffset;
	sphereSubGeo.indexCount = (UINT)sphereMesh.Indices32.size();
	sphereSubGeo.startIndexLocation = sphereMeshIndexOffset;

	//assemble vertices and indices
	mMeshGeo.reset(new MeshGeometry);
	D3DCreateBlob(vertices.size() * sizeof(Vertex), mMeshGeo->VertexBufferCPU.GetAddressOf());
	memcpy(mMeshGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vertices.size() * sizeof(Vertex));
	mMeshGeo->VertexBufferGPU =
	D3DUtil::CreateDefaultBuffer(mDevice.Get(), mCmdList.Get(), vertices.data(), vertices.size() * sizeof(Vertex), mMeshGeo->VertexBufferUploader);
	
	D3DCreateBlob(indices.size() * sizeof(uint32), mMeshGeo->IndexBufferCPU.GetAddressOf());
	memcpy(mMeshGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), indices.size() * sizeof(uint32));
	mMeshGeo->IndexBufferGPU = 
	D3DUtil::CreateDefaultBuffer(mDevice.Get(), mCmdList.Get(), indices.data(), indices.size() * sizeof(uint32), mMeshGeo->IndexBufferUploader);
	
	mMeshGeo->vertexBufferSize = (UINT)vertices.size() * sizeof(Vertex);
	mMeshGeo->vertexBufferStride = sizeof(Vertex);
	mMeshGeo->indexBufferSize = (UINT)indices.size() * sizeof(uint32);
	mMeshGeo->indexBufferFormat = DXGI_FORMAT_R32_UINT;
	mMeshGeo->Name = "Shape";
	mMeshGeo->drawArgs["Box"] = std::move(boxSubGeo);
	mMeshGeo->drawArgs["Cylinder"] = std::move(cylinderSubGeo);
	mMeshGeo->drawArgs["Sphere"] = std::move(sphereSubGeo);

	return true;
}

bool LightApp::BuildRenderItems()
{
	using namespace DirectX;
	SubMeshGeometry& box = mMeshGeo->drawArgs["Box"];
	SubMeshGeometry& cylinder = mMeshGeo->drawArgs["Cylinder"];
	SubMeshGeometry& sphere = mMeshGeo->drawArgs["Sphere"];

	UINT cbIndex = 0;

	//floor
	XMFLOAT3 planeLocation = { 0.f, -2.f, 0.f };
	RenderItem floor = {};
	floor.BaseVertexLocation = box.baseVertexLocation;
	floor.DrawIndexCount = box.indexCount;
	floor.DrawStartIndex = box.startIndexLocation;
	floor.GeoMesh = mMeshGeo.get();
	XMFLOAT4X4 boxModel = {};
	XMStoreFloat4x4(&boxModel, XMMatrixTranslation(planeLocation.x, planeLocation.y, planeLocation.z));
	floor.ModelMat = boxModel;
	floor.NumFramesDirty = mNumFrameResources;
	floor.ObjCBIndex = cbIndex;
	++cbIndex;
	floor.PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mOpaqueRenderItems.push_back(std::move(floor));

	//cylinder
	const int numCylinders = 4;
	XMFLOAT3 cylinderLocations[numCylinders] = { {15.f, 10.f, 15.f}, {15.f, 10.f, -15.f}, {-15.f, 10.f, -15.f}, {-15.f, 10.f, 15.f} };
	XMFLOAT3 sphereLocations[numCylinders] = { {15.f, 21.f, 15.f}, {15.f, 21.f, -15.f}, {-15.f, 21.f, -15.f}, {-15.f, 21.f, 15.f} };
	for (int i = 0; i < numCylinders; ++i)
	{
		//cylinder
		RenderItem cylinderItem = {};
		cylinderItem.BaseVertexLocation = cylinder.baseVertexLocation;
		cylinderItem.DrawIndexCount = cylinder.indexCount;
		cylinderItem.DrawStartIndex = cylinder.startIndexLocation;
		cylinderItem.GeoMesh = mMeshGeo.get();
		XMFLOAT4X4 cylinderModel = {};
		XMStoreFloat4x4(&cylinderModel, XMMatrixTranslation(cylinderLocations[i].x, cylinderLocations[i].y, cylinderLocations[i].z));
		cylinderItem.ModelMat = cylinderModel;
		cylinderItem.NumFramesDirty = 3;
		cylinderItem.ObjCBIndex = cbIndex;
		++cbIndex;
		cylinderItem.PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		mOpaqueRenderItems.push_back(std::move(cylinderItem));

		//sphere
		RenderItem sphereItem = {};
		sphereItem.BaseVertexLocation = sphere.baseVertexLocation;
		sphereItem.DrawIndexCount = sphere.indexCount;
		sphereItem.DrawStartIndex = sphere.startIndexLocation;
		sphereItem.GeoMesh = mMeshGeo.get();
		XMFLOAT4X4 sphereModel = {};
		XMStoreFloat4x4(&sphereModel, XMMatrixTranslation(sphereLocations[i].x, sphereLocations[i].y, sphereLocations[i].z));
		sphereItem.ModelMat = sphereModel;
		sphereItem.NumFramesDirty = 3;
		sphereItem.ObjCBIndex = cbIndex;
		++cbIndex;
		sphereItem.PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		mOpaqueRenderItems.push_back(std::move(sphereItem));
	}

	return true;
}

bool LightApp::InitCamera()
{
	mCamera.reset(new Camera(DirectX::XMFLOAT3(0, 20, -50), 0.f, 0.f, 45.f, (float)mMainWindow->getWidth() / (float)mMainWindow->getHeight()));
	return true;
}

void LightApp::UpdateObjectCB()
{
	for (int i = 0; i < mOpaqueRenderItems.size(); ++i)
	{
		if (mOpaqueRenderItems[i].NumFramesDirty > 0)
		{
			ObjectConstant objConst = {};
			objConst.Model = mOpaqueRenderItems[i].ModelMat;
			DirectX::XMMATRIX modelMat = DirectX::XMLoadFloat4x4(&objConst.Model);
			DirectX::XMMATRIX invModelMat = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(modelMat), modelMat);
			modelMat = DirectX::XMMatrixTranspose(modelMat);
			invModelMat = DirectX::XMMatrixTranspose(invModelMat);
			DirectX::XMStoreFloat4x4(&objConst.Model, modelMat);
			DirectX::XMStoreFloat4x4(&objConst.InvModel, invModelMat);
			mCurFrameResource->ObjectCB->CopyData(i, objConst);
			--mOpaqueRenderItems[i].NumFramesDirty;
		}
	}
}

void LightApp::UpdatePassCB()
{
	using namespace DirectX;
	PassConstant passConst = {};

	XMMATRIX view = XMLoadFloat4x4(&mCamera->GetViewMatrix());
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX proj = XMLoadFloat4x4(&mCamera->GetProjectMatrix());
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX viewProj = XMLoadFloat4x4(&mCamera->GetViewProjMatrix());
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);
	
	view = XMMatrixTranspose(view);
	invView = XMMatrixTranspose(invView);
	proj = XMMatrixTranspose(proj);
	invProj = XMMatrixTranspose(invProj);
	viewProj = XMMatrixTranspose(viewProj);
	invViewProj = XMMatrixTranspose(invViewProj);

	XMStoreFloat4x4(&passConst.View, view);
	XMStoreFloat4x4(&passConst.InvView, invView);
	XMStoreFloat4x4(&passConst.Proj, proj);
	XMStoreFloat4x4(&passConst.InvProj, invProj);
	XMStoreFloat4x4(&passConst.ViewProj, viewProj);
	XMStoreFloat4x4(&passConst.InvViewProj, invViewProj);

	passConst.EyePosW = mCamera->GetLocation();
	passConst.RenderTargetSize = {(float)mMainWindow->getWidth(), (float)mMainWindow->getHeight()};
	passConst.InvRenderTargetSize = {(float)(1.f / mMainWindow->getWidth()), (float)(1.f / mMainWindow->getHeight())};
	passConst.NearZ = mCamera->GetNear();
	passConst.FarZ = mCamera->GetFar();
	passConst.TotalTime = (float)mGlobalTimer->totalTime();
	passConst.DeltaTime = (float)mGlobalTimer->deltaTime();

	mCurFrameResource->PassCB->CopyData(0, passConst);
}

void LightApp::DrawItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem>& renderItems)
{
	UINT objCBVIndex = 0;
	for (int i = 0; i < renderItems.size(); ++i)
	{
		D3D12_GPU_VIRTUAL_ADDRESS cbAdress = mFrameResources[mCurrentBackBufferIndex].ObjectCB->GetElementGUPVirtualAddress(i);
		cmdList->SetGraphicsRootConstantBufferView(1, cbAdress);

		cmdList->IASetVertexBuffers(0, 1, &renderItems[i].GeoMesh->vertexBufferView());
		cmdList->IASetIndexBuffer(&renderItems[i].GeoMesh->indexBufferView());
		cmdList->IASetPrimitiveTopology(renderItems[i].PrimitiveTopology);

		cmdList->DrawIndexedInstanced(renderItems[i].DrawIndexCount, 1, renderItems[i].DrawStartIndex, renderItems[i].BaseVertexLocation, 0);
	}
}
bool LightApp::BuildLights()
{
	//mDirectionalLights.push_back(DirectionalLight({1.f, 1.f, 1.f}, {-1.f, -1.f, -1.f}));
	mPointLights.push_back(PointLight({1.f, 1.f, 1.f}, {0.f, 10.f, 0.f}, 0.1f, 30.f));
	mSpotLights.push_back(SpotLight({ 0.f, 1.f, 0.f }, { 0.f, 10.f, 0.f }, { -1.f, - 1.f, -1.f }, 0.1f, 100.f, 5.f, 45.f));
	return true;
}

void LightApp::UpdateLightCB()
{
	LightConstant lightConst = {};
	int numLight = 0;
	for (int i = 0; i < (int)mDirectionalLights.size(); ++i, ++numLight)
	{
		if (numLight < MAX_LIGHT_COUNT)
			lightConst.lights[i] = mDirectionalLights[i].GetLightConstant();
	}

	for (int i = 0; i < (int)mPointLights.size(); ++i, ++numLight)
	{
		if(numLight < MAX_LIGHT_COUNT)
			lightConst.lights[i + (int)mDirectionalLights.size()] =  mPointLights[i].GetLightConstant();
	}

	for (int i = 0; i < (int)mSpotLights.size(); ++i, ++numLight)
	{
		if(numLight < MAX_LIGHT_COUNT)
			lightConst.lights[i + (int)mDirectionalLights.size() + (int)mPointLights.size()] = mSpotLights[i].GetLightConstant();
	}
	mCurFrameResource->LightCB->CopyData(0, lightConst);
}


bool LightApp::BuildRootSignature()
{
	const int NUM_ROOT_PARAM = 3;
	CD3DX12_ROOT_PARAMETER cbvParam[NUM_ROOT_PARAM];
	cbvParam[0].InitAsConstantBufferView(0);
	cbvParam[1].InitAsConstantBufferView(1);
	cbvParam[2].InitAsConstantBufferView(2);

	Microsoft::WRL::ComPtr<ID3D10Blob> rootSigBlob;
	HRESULT result = D3D12SerializeRootSignature(&CD3DX12_ROOT_SIGNATURE_DESC(NUM_ROOT_PARAM, cbvParam, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),
		D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob, nullptr);
	if (FAILED(result))
		return false;

	result = mDevice->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSig));
	return SUCCEEDED(result);
}

bool LightApp::BuildShaders()
{
	mVSShader.reset(new D3DShader(L"./Shaders/base_vs.hlsl", "vs_5_0", nullptr));
	mPSShader.reset(new D3DShader(L"./Shaders/base_ps.hlsl", "ps_5_0", nullptr));

	return true;
}

bool LightApp::BuildPSO()
{
	auto psoDesc = D3DUtil::CreateDefaultPSO();

	D3D12_INPUT_ELEMENT_DESC ied[4] = {
		{"V_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"V_NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"V_TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"V_TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	D3D12_INPUT_LAYOUT_DESC inputDesc = {};
	inputDesc.NumElements = 4;
	inputDesc.pInputElementDescs = ied;

	psoDesc.pRootSignature = mRootSig.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(mVSShader->GetBlob()->GetBufferPointer(), mVSShader->GetBlob()->GetBufferSize());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(mPSShader->GetBlob()->GetBufferPointer(), mPSShader->GetBlob()->GetBufferSize());
	psoDesc.InputLayout = inputDesc;
	psoDesc.SampleDesc.Count = m4xMsaaActive ? 4 : 1;
	psoDesc.SampleDesc.Quality = m4xMsaaActive ? queryMsaaQuality() - 1 : 0;
	//psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	HRESULT result = mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mOpaquePSO));
	return SUCCEEDED(result);
}

void LightApp::Update(float deltaTime)
{
	D3DApp::Update(deltaTime);
	mCurFrameResourceIndex = (mCurFrameResourceIndex + 1) % mNumFrameResources;
	mCurFrameResource = &mFrameResources[mCurFrameResourceIndex];

	if (mCurFrameResource->FenceValue != 0 && mFence->GetCompletedValue() < mCurFrameResource->FenceValue)
	{
		HANDLE hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		mFence->SetEventOnCompletion(mCurFrameResource->FenceValue, hEvent);
		if (hEvent)
		{
			WaitForSingleObject(hEvent, INFINITE);
			CloseHandle(hEvent);
		}	
	}

	UpdatePassCB();
	UpdateObjectCB();
	UpdateLightCB();
}

void LightApp::Draw()
{
	D3DApp::Draw();
	mCurFrameResource->CmdAlloc->Reset();
	mCmdList->Reset(mCurFrameResource->CmdAlloc.Get(), mOpaquePSO.Get());

	CmdListOpenRtvAndDsv(mCmdList.Get(), mBackBufferTextures[mCurrentBackBufferIndex].Get(), mDepthStencilTexture.Get());
	CmdListSetVptAndSciRct(mCmdList.Get());
	float clearColor[4] = {0.f, 0.f, 0.f, 1.f};
	CmdListClearRtvAndDsv(mCmdList.Get(), CurrentBackBufferRTV(), currentDepthStencilDSV(), clearColor, 1.f, 0xff);
	mCmdList->OMSetRenderTargets(1, &CurrentBackBufferRTV(), true, &currentDepthStencilDSV());

	mCmdList->SetGraphicsRootSignature(mRootSig.Get());
	mCmdList->SetGraphicsRootConstantBufferView(0, mCurFrameResource->PassCB->GetElementGUPVirtualAddress(0));
	mCmdList->SetGraphicsRootConstantBufferView(2, mCurFrameResource->LightCB->GetElementGUPVirtualAddress(0));

	DrawItems(mCmdList.Get(), mOpaqueRenderItems);

	CmdListCloseRtvAndDsv(mCmdList.Get(), mBackBufferTextures[mCurrentBackBufferIndex].Get(), mDepthStencilTexture.Get());
	mCmdList->Close();
	mCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList*const*)mCmdList.GetAddressOf());

	mCurFrameResource->FenceValue = ++mCurrentFenceValue;
	mCmdQueue->Signal(mFence.Get(), mCurrentFenceValue);

	mDxgiSwapChain->Present(0, 0);
	SwapBackBuffer();
}

void LightApp::OnKeyboardAxisEvent(KEY_TYPE key)
{
	float deltaTime = (float)mGlobalTimer->deltaTime();
	float moveDistance = mMoveSpeed * deltaTime;
	switch (key)
	{
	case KEY_TYPE::KEY_W:
		mCamera->MoveFoward(moveDistance);
		break;
	case KEY_TYPE::KEY_A:
		mCamera->MoveLeft(moveDistance);
		break;
	case KEY_TYPE::KEY_S:
		mCamera->MoveBack(moveDistance);
		break;
	case KEY_TYPE::KEY_D:
		mCamera->MoveRight(moveDistance);
		break;
	case KEY_TYPE::KEY_Q:
		mCamera->MoveDown(moveDistance);
		break;
	case KEY_TYPE::KEY_E:
		mCamera->MoveUp(moveDistance);
		break;
	default:
		break;
	}
}

void LightApp::OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState)
{
	if (mouseButton == MOUSE_BUTTON_TYPE::BUTTON_RIGHT)
	{
		mMouseRightButtonDown = (pState == PRESS_STATE::PRESS_DOWN) ? true : false;
	}
}

void LightApp::OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed)
{
	if (mMouseRightButtonDown)
	{
		const float turnRate = 0.2f;
		float deltaTime = (float)mGlobalTimer->deltaTime();
		float turnAngle = turnRate * deltaTime;
		mCamera->TurnRight(xSpeed * turnAngle);
		mCamera->TurnUp(ySpeed * turnAngle);

		if (zSpeed > 0.f)
		{
			mMoveSpeed *= 1.1f;
		}
		else if (zSpeed < 0.f)
		{
			mMoveSpeed *= 0.9f;
		}
	}
}

void LightApp::CmdListClearRtvAndDsv(ID3D12GraphicsCommandList* cmdList, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv, float clearColor[4], float depth, unsigned char stencil)
{
	cmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
}

void LightApp::CmdListSetVptAndSciRct(ID3D12GraphicsCommandList* cmdList)
{
	D3D12_VIEWPORT vp = { 0, 0, (float)mMainWindow->getWidth(), (float)mMainWindow->getHeight(), 0.f, 1.f };
	D3D12_RECT sciRect = { 0, 0, (LONG)mMainWindow->getWidth(), (LONG)mMainWindow->getHeight() };
	cmdList->RSSetViewports(1, &vp);
	cmdList->RSSetScissorRects(1, &sciRect);
}

void LightApp::CmdListOpenRtvAndDsv(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* rtv, ID3D12Resource* dsv)
{
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtv, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(dsv, D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void LightApp::CmdListCloseRtvAndDsv(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* rtv, ID3D12Resource* dsv)
{
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtv, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(dsv, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ));
}
