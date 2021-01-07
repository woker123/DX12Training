#include "WaveApp.h"
#include "D3DUtil.h"
#include "MathHelper.h"

bool WaveApp::InitializeApp(HINSTANCE hInstance)
{
	if (!D3DApp::InitializeApp(hInstance))
		return false;

	mCmdAllacator->Reset();
	mCmdList->Reset(mCmdAllacator.Get(), nullptr);

	if (!InitCamere())
		return false;

	if (!InitMeshGeo())
		return false;

	if (!InitRenderItems())
		return false;

	if (!InitShaders())
		return false;

	if (!InitFrameResources())
		return false;

	if (!InitRootSignature())
		return false;

	if (!InitPSO())
		return false;

	mCmdList->Close();
	mCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)mCmdList.GetAddressOf());
	FlushCommandQueue();

	return true;
}

void WaveApp::OnKeyboardAxisEvent(KEY_TYPE key)
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

void WaveApp::OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState)
{
	if (mouseButton == MOUSE_BUTTON_TYPE::BUTTON_RIGHT)
	{
		mMouseRightButtonDown = (pState == PRESS_STATE::PRESS_DOWN) ? true : false;
	}
}

void WaveApp::OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed)
{
	if (mMouseRightButtonDown)
	{
		const float turnRate = 0.0005f;
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

void WaveApp::Update(float deltaTime)
{
	D3DApp::Update(deltaTime);

	mCurFrameResourceIndex = (mCurFrameResourceIndex + 1) % mNumFrameResource;
	FrameResource* curFrameResource = &mFrameResources[mCurFrameResourceIndex];

	if (curFrameResource->FenceValue > 0 && mFence->GetCompletedValue() < curFrameResource->FenceValue)
	{
		HANDLE hEvent = CreateEventEx(NULL, nullptr, 0, EVENT_ALL_ACCESS);
		mFence->SetEventOnCompletion(curFrameResource->FenceValue, hEvent);
		if (hEvent)
		{
			WaitForSingleObject(hEvent, INFINITE);
			CloseHandle(hEvent);
		}	
	}

	UpdatePassCB(curFrameResource);
	UpdateObjectCB(curFrameResource);
}

void WaveApp::Draw()
{
	D3DApp::Draw();

	FrameResource* curFrameResource = &mFrameResources[mCurFrameResourceIndex];

	curFrameResource->CmdAlloc->Reset();
	mCmdList->Reset(curFrameResource->CmdAlloc.Get(), mPSO.Get());
	
	mCmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.f, 0.f, (float)mMainWindow->getWidth(), (float)mMainWindow->getHeight(), 0.f, 1.f));
	mCmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, mMainWindow->getWidth(), mMainWindow->getHeight()));
	mCmdList->OMSetRenderTargets(1, &CurrentBackBufferRTV(), true, &currentDepthStencilDSV());

	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBackBufferTextures[mCurrentBackBufferIndex].Get(), 
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilTexture.Get(), 
		D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	float clearColor[4] = {1, 1, 1, 1};
	mCmdList->ClearRenderTargetView(CurrentBackBufferRTV(), clearColor, 0, nullptr);
	mCmdList->ClearDepthStencilView(currentDepthStencilDSV(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0xff, 0, nullptr);

	mCmdList->SetGraphicsRootSignature(mRootSignature.Get());
	mCmdList->SetGraphicsRootConstantBufferView(0, mFrameResources[mCurFrameResourceIndex].PassCB->GetElementGUPVirtualAddress(0));
	DrawRenderItems();

	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBackBufferTextures[mCurrentBackBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilTexture.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ));

	mCmdList->Close();
	mCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList*const*)mCmdList.GetAddressOf());
	mDxgiSwapChain->Present(0, 0);
	SwapBackBuffer();

	mCmdQueue->Signal(mFence.Get(), ++mCurrentFenceValue);
	curFrameResource->FenceValue = mCurrentFenceValue;

}

bool WaveApp::InitCamere()
{
	mCamera.reset(new Camera({0.f, 0.f, -10.f}, 0.f, 0.f, 45.f, (float)mMainWindow->getWidth() / (float)mMainWindow->getHeight()));
	return true;
}

bool WaveApp::InitMeshGeo()
{
	mMeshGeo.reset(new MeshGeometry);
	MeshData boxMesh = GeometryGenerator::GenerateBox(1.f, 1.f, 1.f);
	const size_t vertexBufferSizeByte = boxMesh.Vertices.size() * sizeof(Vertex);
	const size_t indexBufferSizeByte = boxMesh.Indices32.size() * sizeof(uint32);

	UINT boxVertexOffset = 0;
	UINT boxVertexStride = sizeof(Vertex);
	UINT boxIndexOffset = 0;
	UINT boxIndexCount = (UINT)boxMesh.Indices32.size();

	D3DCreateBlob(vertexBufferSizeByte, &mMeshGeo->VertexBufferCPU);
	memcpy(mMeshGeo->VertexBufferCPU->GetBufferPointer(), boxMesh.Vertices.data(), vertexBufferSizeByte);

	D3DCreateBlob(indexBufferSizeByte, &mMeshGeo->IndexBufferCPU);
	memcpy(mMeshGeo->IndexBufferCPU->GetBufferPointer(), boxMesh.Indices32.data(), indexBufferSizeByte);

	mMeshGeo->VertexBufferGPU = D3DUtil::CreateDefaultBuffer(mDevice.Get(), mCmdList.Get(), boxMesh.Vertices.data(), vertexBufferSizeByte, mMeshGeo->VertexBufferUploader);
	mMeshGeo->IndexBufferGPU = D3DUtil::CreateDefaultBuffer(mDevice.Get(), mCmdList.Get(), boxMesh.Indices32.data(), indexBufferSizeByte, mMeshGeo->IndexBufferUploader);

	SubMeshGeometry boxSubGeo;
	boxSubGeo.baseVertexLocation = boxVertexOffset;
	boxSubGeo.indexCount = boxIndexCount;
	boxSubGeo.startIndexLocation = boxIndexOffset;

	mMeshGeo->Name = "Static Geo";
	mMeshGeo->vertexBufferSize = (UINT)vertexBufferSizeByte;
	mMeshGeo->vertexBufferStride = boxVertexStride;
	mMeshGeo->indexBufferFormat = DXGI_FORMAT_R32_UINT;
	mMeshGeo->indexBufferSize = (UINT)indexBufferSizeByte;
	mMeshGeo->drawArgs["Box"] = std::move(boxSubGeo);

	return true;
}

bool WaveApp::InitRenderItems()
{
	auto& boxSubGeo = mMeshGeo->drawArgs["Box"];
	UINT objCbIndex = 0;

	RenderItem boxRenderItem;
	boxRenderItem.BaseVertexLocation = boxSubGeo.baseVertexLocation;
	boxRenderItem.DrawIndexCount = boxSubGeo.indexCount;
	boxRenderItem.DrawStartIndex = boxSubGeo.startIndexLocation;
	boxRenderItem.GeoMesh = mMeshGeo.get();
	DirectX::XMStoreFloat4x4(&boxRenderItem.ModelMat, DirectX::XMMatrixIdentity());
	boxRenderItem.NumFramesDirty = 3;
	boxRenderItem.ObjCBIndex = ++objCbIndex;
	boxRenderItem.PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	mRenderItems.push_back(std::move(boxRenderItem));

	return true;
}

bool WaveApp::InitShaders()
{
	mVS.reset(new D3DShader(L"./Shaders/base_vs.hlsl", "vs_5_0"));
	mPS.reset(new D3DShader(L"./Shaders/base_ps.hlsl", "ps_5_0"));
	return true;
}

bool WaveApp::InitFrameResources()
{
	for (UINT i = 0; i < mNumFrameResource; ++i)
	{
		mFrameResources.push_back(FrameResource(mDevice.Get(), 1, (UINT)mRenderItems.size()));
	}
	
	return true;
}

bool WaveApp::InitRootSignature()
{
	CD3DX12_ROOT_PARAMETER rootParam[2] = {};

	rootParam[0].InitAsConstantBufferView(0);
	rootParam[1].InitAsConstantBufferView(1);

	Microsoft::WRL::ComPtr<ID3D10Blob> sigBlob;
	Microsoft::WRL::ComPtr<ID3D10Blob> errBlob;
	HRESULT result = D3D12SerializeRootSignature(&CD3DX12_ROOT_SIGNATURE_DESC(2, rootParam, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),
		D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
	if (FAILED(result))
	{
		OutputDebugStringA((LPCSTR)errBlob->GetBufferPointer());
		return false;
	}
	
	result = mDevice->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));

	return SUCCEEDED(result);
}

bool WaveApp::InitPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso =
		D3DUtil::CreateDefaultPSO();

	D3D12_INPUT_ELEMENT_DESC eleDescs[4] = {
		{"V_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"V_NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"V_TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"V_TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };
	D3D12_INPUT_LAYOUT_DESC inLayoutDesc;
	inLayoutDesc.NumElements = 4;
	inLayoutDesc.pInputElementDescs = eleDescs;

	pso.InputLayout = inLayoutDesc;
	pso.pRootSignature = mRootSignature.Get();
	pso.SampleDesc.Count = m4xMsaaActive ? 4 : 1;
	pso.SampleDesc.Quality = m4xMsaaActive ? queryMsaaQuality() - 1 : 0;
	pso.VS = CD3DX12_SHADER_BYTECODE(mVS->GetBlob());
	pso.PS = CD3DX12_SHADER_BYTECODE(mPS->GetBlob());

	HRESULT result =
		mDevice->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&mPSO));

	return SUCCEEDED(result);
}

void WaveApp::UpdatePassCB(FrameResource* frameResource)
{
	using namespace DirectX;
	PassConstant passConst = {};
	passConst.DeltaTime = (float)mGlobalTimer->deltaTime();
	passConst.EyePosW = mCamera->GetLocation();
	passConst.FarZ = mCamera->GetFar();
	passConst.InvProj = MathHelper::Transpose(mCamera->GetInvProjMatrix());
	passConst.InvRenderTargetSize = { 1.f / (float)mMainWindow->getWidth(), 1.f / (float)mMainWindow->getHeight() };
	passConst.InvView = MathHelper::Transpose(mCamera->GetInvViewMatrix());
	passConst.InvViewProj = MathHelper::Transpose(mCamera->GetInvViewProjMatrix());
	passConst.NearZ = mCamera->GetNear();
	passConst.Proj = MathHelper::Transpose(mCamera->GetProjectMatrix());
	passConst.RenderTargetSize = XMFLOAT2((float)mMainWindow->getWidth(), (float)mMainWindow->getHeight());
	passConst.TotalTime = (float)mGlobalTimer->totalTime();
	passConst.View = MathHelper::Transpose(mCamera->GetViewMatrix());
	passConst.ViewProj = MathHelper::Transpose(mCamera->GetViewProjMatrix());
	
	frameResource->PassCB->CopyData(0, passConst);
}

void WaveApp::UpdateObjectCB(FrameResource* frameResource)
{
	using namespace DirectX;
	for (int i = 0; i < (int)mRenderItems.size(); ++i)
	{
		if (mRenderItems[i].NumFramesDirty > 0)
		{
			ObjectConstant objConst = {};
			objConst.Model = MathHelper::Transpose(mRenderItems[i].ModelMat);
			XMMATRIX invModel = XMLoadFloat4x4(&mRenderItems[i].ModelMat);
			invModel = XMMatrixInverse(&XMMatrixDeterminant(invModel), invModel);
			invModel = XMMatrixTranspose(invModel);
			XMFLOAT4X4 invM;
			XMStoreFloat4x4(&invM, invModel);
			objConst.Model = MathHelper::Transpose(invM);
			frameResource->ObjectCB->CopyData(i, objConst);
			--mRenderItems[i].NumFramesDirty;
		}
	}
}

void WaveApp::DrawRenderItems()
{
	for (int i = 0; i < (int)mRenderItems.size(); ++i)
	{
		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = mMeshGeo->VertexBufferGPU->GetGPUVirtualAddress();
		vbv.SizeInBytes = mMeshGeo->vertexBufferSize;
		vbv.StrideInBytes = mMeshGeo->vertexBufferStride;

		D3D12_INDEX_BUFFER_VIEW idv = {};;
		idv.BufferLocation = mMeshGeo->IndexBufferGPU->GetGPUVirtualAddress();
		idv.SizeInBytes = mMeshGeo->indexBufferSize;
		idv.Format = DXGI_FORMAT_R32_UINT;

		mCmdList->IASetVertexBuffers(0, 1, &vbv);
		mCmdList->IASetIndexBuffer(&idv);
		mCmdList->IASetPrimitiveTopology(mRenderItems[i].PrimitiveTopology);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAdress = mFrameResources[mCurFrameResourceIndex].ObjectCB->GetElementGUPVirtualAddress(i);
		mCmdList->SetGraphicsRootConstantBufferView(1, objCBAdress);

		mCmdList->DrawIndexedInstanced(mRenderItems[i].DrawIndexCount, 1, mRenderItems[i].DrawStartIndex, mRenderItems[i].BaseVertexLocation, 0);
	}

}
