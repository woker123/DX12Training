#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <dxgi.h>
#include <d3d12.h>
#include "GameTimer.h"
#include "GraphicWindow.h"
#include <memory>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class D3DApp
{
public:
	D3DApp();
	virtual ~D3DApp();
	D3DApp(const D3DApp&) = delete;
	D3DApp& operator=(const D3DApp&) = delete;

public:
	int Run();
	void EnableDebugLayer();

	void Set4xMSAA(bool msaaState);
	bool Get4xMSAA();

	virtual bool InitializeApp(HINSTANCE hInstance);

protected:
	virtual bool CreateRtvAndDsvHeap();
	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual void OnResize(int newWidth, int newHeight);

protected:
	bool InitializeMainWindow();
	bool InitializeD3D12();
	bool CreateD3DFactoryAndDevice();
	bool CreateCommandObjects();
	bool CreateSwapchain();
	bool CreateRTVAndDSV();
	bool CreateFence();
	bool CreateGlobalTimer();

	void FlushCommandQueue();
	int  getRTVDescriptorIncreaseSize();
	int  getDSVDescriptorIncreaseSize();
	void SwapBackBuffer();
	D3D12_CPU_DESCRIPTOR_HANDLE currentBackBufferRTV();
	D3D12_CPU_DESCRIPTOR_HANDLE currentDepthStencilDSV();
	int  queryMsaaQuality();

protected:
	Microsoft::WRL::ComPtr<IDXGIFactory1> mDxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mDxgiSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> mDevice;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCmdQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCmdAllacator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCmdList;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mBackBufferRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDepthStencilDsvHeap;
	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	Microsoft::WRL::ComPtr<ID3D12Resource> mBackBufferTextures[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStenceilTexture;

protected:
	HINSTANCE mHInstance;
	std::shared_ptr<GraphicWindow> mMainWindow;
	std::shared_ptr<GameTimer> mGlobalTimer;
	bool m4xMsaaActive = false;
	int  m4xMsaaQuality = 0;
	bool mIsAppActive = true;
	bool mIsAppMinimal = false;
	int  mCurrentBackBuffer = 0;
	UINT64 mCurrentFenceValue = 0;
};

