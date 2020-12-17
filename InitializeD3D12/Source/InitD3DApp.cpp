#include "InitD3DApp.h"
#include "d3dx12.h"
#include <D3DUtil.h>

void InitD3DApp::Draw()
{
	D3DApp::Draw();

	D3D12_VIEWPORT vp = {};
	vp.Height = (float)mMainWindow->getHeight();
	vp.Width = (float)mMainWindow->getWidth();
	vp.MaxDepth = 1.0;
	vp.MinDepth = 0.0;

	mCmdAllacator->Reset();
	mCmdList->Reset(mCmdAllacator.Get(), nullptr);

	mCmdList->RSSetViewports(1, &vp);
	float color[4] = { 0.1f, 0.5f, 1.0f, 1.0f };
	ClearRTVAndDSV(mCmdList.Get(), color, 1.0, 0xff);
	
	mCmdList->Close();
	mCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList*const*)mCmdList.GetAddressOf());
	mDxgiSwapChain->Present(0, 0);  

	SwapBackBuffer();	
	FlushCommandQueue();
}

void InitD3DApp::ClearRTVAndDSV(ID3D12GraphicsCommandList* cmdList, float color[4], float depth, unsigned char stencil)
{
	//clear rendertarget
	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBackBufferTextures[mCurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	mCmdList->ClearRenderTargetView(currentBackBufferRTV(), color, 0, nullptr);
	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBackBufferTextures[mCurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//clear depth stencil
	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStenceilTexture.Get(),
		D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	mCmdList->ClearDepthStencilView(currentDepthStencilDSV(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0, nullptr);
	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStenceilTexture.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ));
}
