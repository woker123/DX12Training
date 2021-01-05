#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <dxgi.h>
#include <d3d12.h>
#include <memory>
#include "GameTimer.h"
#include "GraphicWindow.h"
#include "InputHandler.h"
#include <string>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DeviceInput;

class D3DApp
{
public:
	D3DApp();
	virtual ~D3DApp();
	D3DApp(const D3DApp&) = delete;
	D3DApp& operator=(const D3DApp&) = delete;

public:
	void Set4xMSAA(bool msaaState);
	bool Get4xMSAA();

	virtual bool InitializeApp(HINSTANCE hInstance);
	int Run();

	virtual void OnKeyboardActionEvent(KEY_TYPE key, PRESS_STATE pState) {}
	virtual void OnKeyboardAxisEvent(KEY_TYPE key) {}
	virtual void OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState) {}
	virtual void OnMouseButtonAxisEvent(MOUSE_BUTTON_TYPE mouseButton) {}
	virtual void OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed) {};

protected:
	virtual bool CreateRtvAndDsvHeap();
	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual void OnResize(int newWidth, int newHeight);
	virtual std::wstring WindowTitile() { return L"Initialize D3D12"; }

protected:
	bool InitializeMainWindow();
	bool InitializeD3D12();
	bool CreateD3DFactoryAndDevice();
	bool CreateCommandObjects();
	bool CreateSwapchain();
	bool CreateRTVAndDSV();
	bool CreateFence();
	bool CreateGlobalTimer();

	void EnableDebugLayer();
	void FlushCommandQueue();
	int  getRTVDescriptorIncreaseSize();
	int  getDSVDescriptorIncreaseSize();
	void SwapBackBuffer();
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferRTV();
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
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilTexture;

protected:
	HINSTANCE mHInstance;
	std::shared_ptr<GraphicWindow> mMainWindow;
	std::shared_ptr<GameTimer> mGlobalTimer;
	int  mCurrentBackBufferIndex = 0;
	bool m4xMsaaActive = false;
	int  m4xMsaaQuality = 0;
	bool mIsAppActive = true;
	bool mIsAppMinimal = false;
	
	UINT64 mCurrentFenceValue = 0;
	std::shared_ptr<InputHanler<D3DApp>> mInput;
};

