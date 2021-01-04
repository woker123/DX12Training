#include "ShapeApp.h"
#include "d3dx12.h"
#include "GeometryGenerator.h"
#include <d3dcompiler.h>
#include "D3DUtil.h"
#include "FrameResource.h"

bool ShapeApp::InitializeApp(HINSTANCE hInstance)
{
	if (!D3DApp::InitializeApp(hInstance))
		return false;

	mCmdAllacator->Reset();
	mCmdList->Reset(mCmdAllacator.Get(), nullptr);

	if (!BuildRootSignature())
		return false;

	if (!BuildShaders())
		return false;

	if (!BuildPSO())
		return false;

	if (!BuildDescriptorHeap())
		return false;

	if (!BuildFrameResources())
		return false;

	if (!BuildCBVs())
		return false;

	if (!BuildMeshGeometry())
		return false;

	if (!BuildRenderItems())
		return false;
	
	if (!InitCamera())
		return false;

	mCmdList->Close();
	mCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList*const*)mCmdList.GetAddressOf());
	FlushCommandQueue();

	return true;
}

bool ShapeApp::BuildFrameResources()
{
	for (int i = 0; i < mNumFrameResources; ++i)
	{
		mFrameResources.push_back(FrameResource(mDevice.Get(), 1, mNumRenderItems));
	}

	return true;
}

bool ShapeApp::BuildCBVs()
{
	UINT objCBSize = D3DUtil::CalcConstantBufferByteSize(sizeof(ObjectConstant));
	auto objCBVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int frameIndex = 0; frameIndex < mNumFrameResources; ++frameIndex)
	{
		for (int objIndex = 0; objIndex < mNumRenderItems; ++objIndex)
		{
			UINT objCBVIndex = frameIndex * mNumRenderItems + objIndex;
			objCBVHandle.Offset(objCBVIndex, mCBVDescriptorSize);
			D3D12_GPU_VIRTUAL_ADDRESS objCBVAdress = mFrameResources[frameIndex].ObjectCB->GetElementGUPVirtualAddress(objIndex);
			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvd = {};
			cbvd.BufferLocation = objCBVAdress;
			cbvd.SizeInBytes = mFrameResources[frameIndex].ObjectCB->GetElementSizeByte();
			mDevice->CreateConstantBufferView(&cbvd, objCBVHandle);
		}
	}

	UINT passCBVIndex = mNumFrameResources * mNumRenderItems;
	for (int frameIndex = 0; frameIndex < mNumFrameResources; ++frameIndex)
	{
		UINT cbvIndex = passCBVIndex + frameIndex;
		D3D12_GPU_VIRTUAL_ADDRESS passCBVAdress = mFrameResources[frameIndex].PassCB->GetElementGUPVirtualAddress(0);
		auto passCBVHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
		passCBVHandle.Offset(cbvIndex, mCBVDescriptorSize);
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = passCBVAdress;
		cbvDesc.SizeInBytes = mFrameResources[frameIndex].PassCB->GetElementSizeByte();
		mDevice->CreateConstantBufferView(&cbvDesc, passCBVHandle);
	}

	return true;
}

bool ShapeApp::BuildMeshGeometry()
{
	auto cylinderMesh = GeometryGenerator::GenerateCylinder(1.f, 2.f, 3.f, 32, 32);
	UINT cylinderVertexOffset = 0;
	UINT cylinderIndexOffset = 0;
	
	SubMeshGeometry cylinderSubGeo;
	cylinderSubGeo.baseVertexLocation = 0;
	cylinderSubGeo.indexCount = (UINT)cylinderMesh.Indices32.size();
	cylinderSubGeo.startIndexLocation = 0;

	mMeshGeo.reset(new MeshGeometry);
	
	D3DCreateBlob(cylinderMesh.Vertices.size() * sizeof(Vertex), mMeshGeo->VertexBufferCPU.GetAddressOf());
	memcpy(mMeshGeo->VertexBufferCPU->GetBufferPointer(), cylinderMesh.Vertices.data(), mMeshGeo->VertexBufferCPU->GetBufferSize());
	mMeshGeo->VertexBufferGPU =
	D3DUtil::CreateDefaultBuffer(mDevice.Get(), mCmdList.Get(), cylinderMesh.Vertices.data(), cylinderMesh.Vertices.size() * sizeof(Vertex), mMeshGeo->VertexBufferUploader);
	
	D3DCreateBlob(cylinderMesh.Indices32.size() * sizeof(uint32), mMeshGeo->IndexBufferCPU.GetAddressOf());
	memcpy(mMeshGeo->IndexBufferCPU->GetBufferPointer(), cylinderMesh.Indices32.data(), mMeshGeo->IndexBufferCPU->GetBufferSize());
	mMeshGeo->IndexBufferGPU = 
	D3DUtil::CreateDefaultBuffer(mDevice.Get(), mCmdList.Get(), cylinderMesh.Indices32.data(), cylinderMesh.Indices32.size() * sizeof(uint32), mMeshGeo->IndexBufferUploader);
	
	mMeshGeo->indexBufferFormat = DXGI_FORMAT_R32_UINT;
	mMeshGeo->indexBufferSize = (UINT)cylinderMesh.Indices32.size() * sizeof(uint32);
	mMeshGeo->Name = "Shape";
	mMeshGeo->vertexBufferSize = (UINT)cylinderMesh.Vertices.size() * sizeof(Vertex);
	mMeshGeo->vertexBufferStride = sizeof(Vertex);
	mMeshGeo->drawArgs["Cylinder"] = cylinderSubGeo;

	return true;
}

bool ShapeApp::BuildRenderItems()
{
	for (int i = 0; i < mNumRenderItems; ++i)
	{
		RenderItem rItem;
		rItem.BaseVertexLocation = mMeshGeo->drawArgs["Cylinder"].baseVertexLocation;
		rItem.DrawIndexCount = mMeshGeo->drawArgs["Cylinder"].indexCount;
		rItem.DrawStartIndex = mMeshGeo->drawArgs["Cylinder"].startIndexLocation;
		rItem.GeoMesh = mMeshGeo.get();
		rItem.PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		DirectX::XMMATRIX model = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat4x4(&rItem.ModelMat, model);
		rItem.NumFramesDirty = mNumFrameResources;
		rItem.ObjCBIndex = i;
		mOpaqueRenderItems.push_back(rItem);
	}

	return true;
}

bool ShapeApp::InitCamera()
{
	mCamera.reset(new Camera(DirectX::XMFLOAT3(0, 0, -10), 0.f, 0.f, 45.f, 1.f));
	return true;
}

void ShapeApp::UpdateObjectCB()
{
	for (int i = 0; i < mNumRenderItems; ++i)
	{
		if (mOpaqueRenderItems[i].NumFramesDirty > 0)
		{
			for (int frameIndex = 0; frameIndex < mNumFrameResources; ++frameIndex)
			{
				ObjectConstant objConst = {};
				objConst.Model = mOpaqueRenderItems[i].ModelMat;
				DirectX::XMMATRIX modelMat = DirectX::XMLoadFloat4x4(&objConst.Model);
				modelMat = DirectX::XMMatrixTranspose(modelMat);
				DirectX::XMStoreFloat4x4(&objConst.Model, modelMat);
				mFrameResources[frameIndex].ObjectCB->CopyData(i, objConst);
				--mOpaqueRenderItems[i].NumFramesDirty;
			}
		}
	}
}

void ShapeApp::UpdatePassCB()
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

void ShapeApp::DrawItems(ID3D12GraphicsCommandList* cmdList, std::vector<RenderItem>& renderItems)
{
	UINT objCBVIndex = mCurFrameResourceIndex * (UINT)renderItems.size();
	for (int i = 0; i < renderItems.size(); ++i)
	{
		auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
		cbvHandle.Offset(objCBVIndex + i, mCBVDescriptorSize);
		cmdList->SetGraphicsRootDescriptorTable(1, cbvHandle);

		cmdList->IASetVertexBuffers(0, 1, &renderItems[i].GeoMesh->vertexBufferView());
		cmdList->IASetIndexBuffer(&renderItems[i].GeoMesh->indexBufferView());
		cmdList->IASetPrimitiveTopology(renderItems[i].PrimitiveTopology);

		cmdList->DrawIndexedInstanced(renderItems[i].DrawIndexCount, 1, renderItems[i].DrawStartIndex, renderItems[i].BaseVertexLocation, 0);
	}
}

bool ShapeApp::BuildDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC dhd = {};
	dhd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	dhd.NodeMask = 0;
	dhd.NumDescriptors = mNumFrameResources * (mNumRenderItems + 1);
	dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	mCBVDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(dhd.Type);

	HRESULT result =
	mDevice->CreateDescriptorHeap(&dhd, IID_PPV_ARGS(&mCbvHeap));
	return SUCCEEDED(result);
}

bool ShapeApp::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE cbvRange1;
	CD3DX12_DESCRIPTOR_RANGE cbvRange2;

	cbvRange1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	cbvRange2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	CD3DX12_ROOT_PARAMETER cbvTable[2];
	cbvTable[0].InitAsDescriptorTable(1, &cbvRange1);
	cbvTable[1].InitAsDescriptorTable(1, &cbvRange2);

	Microsoft::WRL::ComPtr<ID3D10Blob> rootSigBlob;
	HRESULT result = D3D12SerializeRootSignature(&CD3DX12_ROOT_SIGNATURE_DESC(2, cbvTable, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),
		D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob, nullptr);
	if (FAILED(result))
		return false;

	result = mDevice->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSig));
	return SUCCEEDED(result);
}

bool ShapeApp::BuildShaders()
{
	mVSShader.reset(new D3DShader(L"./Shaders/base_vs.hlsl", "vs_5_0"));
	mPSShader.reset(new D3DShader(L"./Shaders/base_ps.hlsl", "ps_5_0"));

	return true;
}

bool ShapeApp::BuildPSO()
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

	HRESULT result = mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mOpaquePSO));
	return SUCCEEDED(result);
}

void ShapeApp::Update(float deltaTime)
{
	D3DApp::Update(deltaTime);





}

void ShapeApp::Draw()
{
	D3DApp::Draw();

	mCmdAllacator->Reset();
	mCmdList->Reset(mCmdAllacator.Get(), mOpaquePSO.Get());

	CmdListSetVptAndSciRct(mCmdList.Get());
	CmdListOpenRtvAndDsv(mCmdList.Get(), mBackBufferTextures[mCurrentBackBufferIndex].Get(), mDepthStenceilTexture.Get());
	float clearColor[4] = { 1.f, 1.f, 0.5f, 1.f };
	CmdListClearRtvAndDsv(mCmdList.Get(), CurrentBackBufferRTV(), currentDepthStencilDSV(), clearColor, 1.f, 0x00);
	mCmdList->OMSetRenderTargets(1, &CurrentBackBufferRTV(), true, &currentDepthStencilDSV());
	
	mCurFrameResource = &mFrameResources[mCurFrameResourceIndex];
	
	mCmdList->SetGraphicsRootSignature(mRootSig.Get());
	mCmdList->SetDescriptorHeaps(0, (ID3D12DescriptorHeap*const*)mCbvHeap.GetAddressOf());
	mCmdList->SetDescriptorHeaps(1, (ID3D12DescriptorHeap*const*)mCbvHeap.GetAddressOf());
	UINT cbvIndex = mNumFrameResources * mNumRenderItems + mCurFrameResourceIndex;
	auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
	cbvHandle.Offset(cbvIndex, mCBVDescriptorSize);
	mCmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);
	
	UpdatePassCB();
	UpdateObjectCB();
	DrawItems(mCmdList.Get(), mOpaqueRenderItems);

	
	CmdListCloseRtvAndDsv(mCmdList.Get(), mBackBufferTextures[mCurrentBackBufferIndex].Get(), mDepthStenceilTexture.Get());
	mCmdList->Close();
	
	mCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList*const*)mCmdList.GetAddressOf());
	FlushCommandQueue();
	mDxgiSwapChain->Present(0, 0);
	SwapBackBuffer();
}

void ShapeApp::OnKeyboardAxisEvent(KEY_TYPE key)
{
	switch (key)
	{
	case KEY_TYPE::KEY_W:
		mCamera->MoveFoward(mMoveSpeed);
		break;
	case KEY_TYPE::KEY_A:
		mCamera->MoveLeft(mMoveSpeed);
		break;
	case KEY_TYPE::KEY_S:
		mCamera->MoveBack(mMoveSpeed);
		break;
	case KEY_TYPE::KEY_D:
		mCamera->MoveRight(mMoveSpeed);
		break;
	case KEY_TYPE::KEY_Q:
		mCamera->MoveDown(mMoveSpeed);
		break;
	case KEY_TYPE::KEY_E:
		mCamera->MoveUp(mMoveSpeed);
		break;
	default:
		break;
	}
}

void ShapeApp::OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState)
{
	if (mouseButton == MOUSE_BUTTON_TYPE::BUTTON_RIGHT)
	{
		mMouseRightButtonDown = (pState == PRESS_STATE::PRESS_DOWN) ? true : false;
	}
}

void ShapeApp::OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed)
{
	if (mMouseRightButtonDown)
	{
		const float turnRate = 0.0001f;
		mCamera->TurnRight(xSpeed * turnRate);
		mCamera->TurnUp(ySpeed * turnRate);

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

void ShapeApp::CmdListClearRtvAndDsv(ID3D12GraphicsCommandList* cmdList, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv, float clearColor[4], float depth, unsigned char stencil)
{
	cmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
}

void ShapeApp::CmdListSetVptAndSciRct(ID3D12GraphicsCommandList* cmdList)
{
	D3D12_VIEWPORT vp = { 0, 0, (float)mMainWindow->getWidth(), (float)mMainWindow->getHeight(), 0.f, 1.f };
	D3D12_RECT sciRect = { 0, 0, (LONG)mMainWindow->getWidth(), (LONG)mMainWindow->getHeight() };
	cmdList->RSSetViewports(1, &vp);
	cmdList->RSSetScissorRects(1, &sciRect);
}

void ShapeApp::CmdListOpenRtvAndDsv(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* rtv, ID3D12Resource* dsv)
{
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtv, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(dsv, D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

void ShapeApp::CmdListCloseRtvAndDsv(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* rtv, ID3D12Resource* dsv)
{
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtv, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(dsv, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ));
}
