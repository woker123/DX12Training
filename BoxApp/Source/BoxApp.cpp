#include "BoxApp.h"
#include "UploadBuffer.h"
#include <DirectXMath.h>
#include "D3DShader.h"
#include "D3DUtil.h"

using namespace DirectX;
bool BoxApp::InitializeApp(HINSTANCE hInstance)
{
    if(!D3DApp::InitializeApp(hInstance))
        return false;
    
    if (!BuildCBVHeap())
        return false;
    
    if (!BuildCBuffer())
        return false;

    if (!BuildRootSignature())
        return false;

    if (!BuildShaderAndInputLayout())
        return false;

    if (!BuildPSO())
        return false;

    if (!BuildMesh())
        return false;

    mCamera.reset(new Camera(XMFLOAT3(0, 0, -5), 0, 0, 45.f, 1.0f));

    return true;
}

void BoxApp::OnKeyboardActionEvent(KEY_TYPE key, PRESS_STATE pState)
{
   
}

void BoxApp::OnKeyboardAxisEvent(KEY_TYPE key)
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

void BoxApp::OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState)
{
    if (mouseButton == MOUSE_BUTTON_TYPE::BUTTON_RIGHT)
    {
        mMouseRightButtonDown = (pState == PRESS_STATE::PRESS_DOWN) ? true : false;
    }
}

void BoxApp::OnMouseButtonAxisEvent(MOUSE_BUTTON_TYPE mouseButton)
{

}

void BoxApp::OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed)
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

bool BoxApp::BuildCBVHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC desHeapDesc = {};
    desHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desHeapDesc.NodeMask = 0;
    desHeapDesc.NumDescriptors = 1;
    desHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    HRESULT result = mDevice->CreateDescriptorHeap(&desHeapDesc, IID_PPV_ARGS(&mCBVHeap));
    return SUCCEEDED(result);
}

bool BoxApp::BuildCBuffer()
{
    mConstBuffer.reset(new UploadBuffer<XMFLOAT4X4>(mDevice.Get(), 1, true));
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = mConstBuffer->GetElementGUPVirtualAddress(0);
    cbvDesc.SizeInBytes = mConstBuffer->GetElementSizeByte();
    mDevice->CreateConstantBufferView(&cbvDesc, mCBVHeap->GetCPUDescriptorHandleForHeapStart());

    return true;
}

bool BoxApp::BuildRootSignature()
{
    CD3DX12_ROOT_PARAMETER slotRootParameter[1] = {};
    CD3DX12_DESCRIPTOR_RANGE descRange;
    descRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    slotRootParameter[0].InitAsDescriptorTable(1, &descRange);

    Microsoft::WRL::ComPtr<ID3D10Blob> rootSigBlob;
    Microsoft::WRL::ComPtr<ID3D10Blob> errBlob;
    HRESULT result = D3D12SerializeRootSignature(
        &CD3DX12_ROOT_SIGNATURE_DESC(1, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT),
        D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob, &errBlob);
    if (FAILED(result))
        OutputDebugStringA((char*)errBlob->GetBufferPointer());

    result = mDevice->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));

    return SUCCEEDED(result);
}

bool BoxApp::BuildShaderAndInputLayout()
{
    D3DShader vs(L"./Shaders/base_vs.hlsl", "vs_5_0");
    D3DShader ps(L"./Shaders/base_ps.hlsl", "ps_5_0");
    mVSBlob = vs.GetBlob();
    mPSBlob = ps.GetBlob();

    mInputLayout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 3 * sizeof(float), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
    };

    return true;
}

bool BoxApp::BuildPSO()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
    pso.InputLayout.NumElements = (UINT)mInputLayout.size();
    pso.InputLayout.pInputElementDescs = mInputLayout.data();
    pso.pRootSignature = mRootSignature.Get();
    pso.VS.BytecodeLength = mVSBlob->GetBufferSize();
    pso.VS.pShaderBytecode = mVSBlob->GetBufferPointer();
    pso.PS.BytecodeLength = mPSBlob->GetBufferSize();
    pso.PS.pShaderBytecode = mPSBlob->GetBufferPointer();

    pso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pso.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    pso.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    pso.NumRenderTargets = 1;
    pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    pso.SampleDesc.Count = m4xMsaaActive ? 4 : 1;
    pso.SampleDesc.Quality = m4xMsaaActive ? (queryMsaaQuality() - 1) : 0;
    pso.SampleMask = UINT_MAX;

    HRESULT result = 
    mDevice->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&mPSO));
    return SUCCEEDED(result);

    return true;
}

bool BoxApp::BuildMesh()
{
    std::vector<Vertex> vertices =
    {
        {{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}},
        {{-1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
        {{+1.0f, +1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
        {{+1.0f, -1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
        {{-1.0f, -1.0f, +1.0f}, {0.0f, 0.0f, 1.0f}},
        {{-1.0f, +1.0f, +1.0f}, {1.0f, 1.0f, 0.0f}},
        {{+1.0f, +1.0f, +1.0f}, {1.0f, 0.0f, 1.0f}},
        {{+1.0f, -1.0f, +1.0f}, {0.5f, 1.0f, 1.0f}}
    };

    std::vector<UINT16> indices = 
    {
        0, 1, 2,
        0, 2, 3,

        4, 6, 5,
        4, 7, 6,

        4, 5, 1,
        4, 1, 0,

        3, 2, 6,
        3, 6, 7,

        1, 5, 6,
        1, 6, 2,

        4, 0, 3, 
        4, 3, 7
    };

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
    
    mCmdAllacator->Reset();
    mCmdList->Reset(mCmdAllacator.Get(), nullptr);
    vertexBuffer = D3DUtil::CreateDefaultBuffer(mDevice.Get(), mCmdList.Get(), vertices.data(), vertices.size() * sizeof(Vertex), mVertexBufferUpload);
    indexBuffer = D3DUtil::CreateDefaultBuffer(mDevice.Get(), mCmdList.Get(), indices.data(), indices.size() * sizeof(UINT16), mIndexBufferUpload);
    mCmdList->Close();
    mCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList*const*)mCmdList.GetAddressOf());
    FlushCommandQueue();

    SubMeshGeometry subGeo = {};
    subGeo.baseVertexLocation = 0;
    subGeo.indexCount = (UINT)indices.size();
    subGeo.startIndexLocation = 0;

    MeshGeometry meshGeo = {};
    meshGeo.drawArgs["Triangle"] = subGeo;
    meshGeo.IndexBufferGPU = indexBuffer;
    meshGeo.indexBufferFormat = DXGI_FORMAT_R16_UINT;
    meshGeo.indexBufferSize = (UINT)indices.size() * sizeof(UINT16);
    meshGeo.VertexBufferGPU = vertexBuffer;
    meshGeo.vertexBufferSize = (UINT)vertices.size() * sizeof(Vertex);
    meshGeo.vertexBufferStride = sizeof(Vertex);
    
    mDrawMesh.reset(new MeshGeometry(meshGeo));
    return true;
}

void BoxApp::Update(float deltaTime)
{
    D3DApp::Update(deltaTime);

    DirectX::XMMATRIX invMat = DirectX::XMLoadFloat4x4(&mCamera->GetViewProjMatrix());
    invMat = DirectX::XMMatrixTranspose(invMat);
    DirectX::XMFLOAT4X4 invM;
    DirectX::XMStoreFloat4x4(&invM, invMat);

    mConstBuffer->CopyData(0, invM);
}

void BoxApp::Draw()
{
    D3DApp::Draw();

    mCmdAllacator->Reset();
    mCmdList->Reset(mCmdAllacator.Get(), mPSO.Get());

    mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBackBufferTextures[mCurrentBackBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
    mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStenceilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
    float clearcolor[4] = { 0.f, 0.f, 0.f, 1.f };
    mCmdList->ClearRenderTargetView(CurrentBackBufferRTV(), clearcolor, 0, nullptr);
    mCmdList->ClearDepthStencilView(currentDepthStencilDSV(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0, 0xff, 0, nullptr);

    D3D12_VIEWPORT vp = {0.f, 0.f, (float)mMainWindow->getWidth(), (float)mMainWindow->getHeight(), 0.0f, 1.0f};
    mCmdList->RSSetViewports(1, &vp);
    mCmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0, mMainWindow->getWidth(), mMainWindow->getHeight()));
    mCmdList->OMSetRenderTargets(1, &CurrentBackBufferRTV(), true, &currentDepthStencilDSV());

    mCmdList->IASetVertexBuffers(0, 1, &mDrawMesh->vertexBufferView());
    mCmdList->IASetIndexBuffer(&mDrawMesh->indexBufferView());
    mCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    mCmdList->SetDescriptorHeaps(1, (ID3D12DescriptorHeap*const*)mCBVHeap.GetAddressOf());
    mCmdList->SetGraphicsRootSignature(mRootSignature.Get());
    mCmdList->SetGraphicsRootDescriptorTable(0, mCBVHeap->GetGPUDescriptorHandleForHeapStart());
    SubMeshGeometry triangleArgs = mDrawMesh->drawArgs["Triangle"];
    mCmdList->DrawIndexedInstanced(triangleArgs.indexCount, 1, triangleArgs.startIndexLocation, triangleArgs.baseVertexLocation, 0);

    mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBackBufferTextures[mCurrentBackBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
    mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStenceilTexture.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ));  
    mCmdList->Close();
    mCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList*const*)mCmdList.GetAddressOf());

    mDxgiSwapChain->Present(0, 0);
    SwapBackBuffer();
    FlushCommandQueue();
}
