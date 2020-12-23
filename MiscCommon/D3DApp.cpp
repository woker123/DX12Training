#include "D3DApp.h"
#include "d3dx12.h"

D3DApp::D3DApp()
    :mHInstance()
{
}

D3DApp::~D3DApp()
{
    FlushCommandQueue();
}

int D3DApp::Run()
{
    mMainWindow->showWindow();
    while (mMainWindow->HandleMessage())
    {
        if (mIsAppActive && !mIsAppMinimal)
        {
            Update((float)mGlobalTimer->deltaTime());
            Draw();
        }
     
    }

    return 0;
}

void D3DApp::EnableDebugLayer()
{
    Microsoft::WRL::ComPtr<ID3D12Debug> debug;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
    debug->EnableDebugLayer();
}

void D3DApp::Set4xMSAA(bool msaaState)
{
    m4xMsaaActive = msaaState;
}

bool D3DApp::Get4xMSAA()
{
    return m4xMsaaActive;
}

bool D3DApp::InitializeApp(HINSTANCE hInstance)
{
#if defined(DEBUG) || defined(_DEBUG) || defined(__DEBUG)
    EnableDebugLayer();
#endif

    mHInstance = hInstance;
    
    if (!InitializeMainWindow())
        return false;

    if (!InitializeD3D12())
        return false;

    if (!CreateGlobalTimer())
        return false;

    m4xMsaaQuality = queryMsaaQuality();
    mGlobalTimer->start();
    mInput.reset(new InputHanler<D3DApp>(hInstance, mMainWindow->getHWND() ,this));
    KEY_TYPE keys[] = {KEY_TYPE::KEY_W, KEY_TYPE::KEY_A, KEY_TYPE::KEY_S, KEY_TYPE::KEY_D, KEY_TYPE::KEY_Q, KEY_TYPE::KEY_E};
    mInput->RegisterKeys(sizeof(keys) / sizeof(keys[0]), keys);

    return true;
}

bool D3DApp::CreateRtvAndDsvHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    desc.NodeMask = 0;
    desc.NumDescriptors = 2;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    HRESULT result = 
    mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mBackBufferRtvHeap));
    if (FAILED(result)) return false;

    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    result = 
    mDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mDepthStencilDsvHeap));
    if (FAILED(result)) return false;

    return true;
}

void D3DApp::Update(float deltaTime)
{
    mGlobalTimer->update();
    mInput->Update((float)mGlobalTimer->deltaTime());
}

void D3DApp::Draw()
{
}

void D3DApp::OnResize(int newWidth, int newHeight)
{
}

bool D3DApp::InitializeMainWindow()
{
    mMainWindow.reset(new GraphicWindow(mHInstance, WindowTitile().data(), 50, 50, 800, 800));
    return true;
}

bool D3DApp::InitializeD3D12()
{
    if (!CreateD3DFactoryAndDevice())
        return false;

    if (!CreateCommandObjects())
        return false;

    if (!CreateSwapchain())
        return false;

    if (!CreateRtvAndDsvHeap())
        return false;

    if (!CreateRTVAndDSV())
        return false;

    if (!CreateFence())
        return false;

    return true;
}

bool D3DApp::CreateD3DFactoryAndDevice()
{
    HRESULT result = 
    CreateDXGIFactory1(IID_PPV_ARGS(&mDxgiFactory));
    if (FAILED(result)) return false;

    result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mDevice));
    if (FAILED(result)) return false;

    return true;
}

bool D3DApp::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
    cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    cmdQueueDesc.NodeMask = 0;
    cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    HRESULT result = mDevice->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&mCmdQueue));
    if (FAILED(result)) return false;

    result = 
    mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCmdAllacator));
    if (FAILED(result)) return false;

    result =
    mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCmdAllacator.Get(), nullptr, IID_PPV_ARGS(&mCmdList));
    if (FAILED(result)) return false;
    mCmdList->Close();
    
    return true;
}

bool D3DApp::CreateSwapchain()
{
    DXGI_SWAP_CHAIN_DESC swpdesc = {};
    swpdesc.BufferCount = 2;
    swpdesc.BufferDesc.Width = mMainWindow->getWidth();
    swpdesc.BufferDesc.Height = mMainWindow->getHeight();
    swpdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swpdesc.BufferDesc.RefreshRate.Denominator = 1;
    swpdesc.BufferDesc.RefreshRate.Numerator = 60;
    swpdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swpdesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swpdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swpdesc.OutputWindow = mMainWindow->getHWND();
    swpdesc.SampleDesc.Count = m4xMsaaActive ? 4 : 1;
    swpdesc.SampleDesc.Quality = m4xMsaaActive ? m4xMsaaQuality : 0;
    swpdesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swpdesc.Windowed = true;
    swpdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    HRESULT result =
        mDxgiFactory->CreateSwapChain(mCmdQueue.Get(), &swpdesc, &mDxgiSwapChain);
    if (FAILED(result)) return false;

    return true;
}

bool D3DApp::CreateRTVAndDSV()
{
    int rtvSize = getRTVDescriptorIncreaseSize();
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandle(mBackBufferRtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (int i = 0; i < 2; ++i)
    {
        mDxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&mBackBufferTextures[i]));
        
        mDevice->CreateRenderTargetView(mBackBufferTextures[i].Get(), nullptr, cpuDescHandle.Offset(i, rtvSize));
    }

    int sampleCount = m4xMsaaActive ? 4 : 1;
    int sampleQuality = m4xMsaaActive ? m4xMsaaQuality : 0;
    D3D12_RESOURCE_DESC dsvDesc = CD3DX12_RESOURCE_DESC(D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, mMainWindow->getWidth(), mMainWindow->getHeight(),
        1, 1, DXGI_FORMAT_D32_FLOAT_S8X24_UINT, sampleCount, sampleQuality, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
    
    HRESULT result =
        mDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &dsvDesc,
            D3D12_RESOURCE_STATE_DEPTH_READ,
            &CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT_S8X24_UINT, 1.0, 0xff),
            IID_PPV_ARGS(&mDepthStenceilTexture)
        );

    if (FAILED(result)) return false;
    mDevice->CreateDepthStencilView(mDepthStenceilTexture.Get(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(mDepthStencilDsvHeap->GetCPUDescriptorHandleForHeapStart()));

    return true;
}

bool D3DApp::CreateFence()
{
    HRESULT result = 
    mDevice->CreateFence(mCurrentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
    if (FAILED(result)) return false;

    return true;
}

bool D3DApp::CreateGlobalTimer()
{
    mGlobalTimer.reset(new GameTimer);
    return true;
}

void D3DApp::FlushCommandQueue()
{
    ++mCurrentFenceValue;
    mCmdQueue->Signal(mFence.Get(), mCurrentFenceValue);

    if (mFence->GetCompletedValue() < mCurrentFenceValue)
    {
        HANDLE hEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);

        HRESULT result =
        mFence->SetEventOnCompletion(mCurrentFenceValue, hEvent);

        if (hEvent)
        {
            WaitForSingleObject(hEvent, INFINITE);
            CloseHandle(hEvent);
        }
        
    }
}

int D3DApp::getRTVDescriptorIncreaseSize()
{
    return mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

int D3DApp::getDSVDescriptorIncreaseSize()
{
    return mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void D3DApp::SwapBackBuffer()
{
    mCurrentBackBuffer = mCurrentBackBuffer == 0 ? 1 : 0;
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::CurrentBackBufferRTV()
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(mBackBufferRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrentBackBuffer, getRTVDescriptorIncreaseSize());
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::currentDepthStencilDSV()
{
    return mDepthStencilDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

int D3DApp::queryMsaaQuality()
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS queryData = {};
    queryData.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    queryData.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    queryData.SampleCount = 4;
    mDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &queryData, sizeof(queryData));

    return queryData.NumQualityLevels;
}

