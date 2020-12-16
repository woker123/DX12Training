#include "InitD3DApp.h"
#include "d3dx12.h"

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
	mCmdList->OMSetRenderTargets(1, &currentBackBufferRTV(), true, &currentDepthStencilDSV());

	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBackBufferTextures[mCurrentBackBuffer].Get(), 
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	float color[4] = {1.0, 1.0, 0.0, 1.0};
	mCmdList->ClearRenderTargetView(currentBackBufferRTV(), color, 0, nullptr);
	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBackBufferTextures[mCurrentBackBuffer].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	
	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStenceilTexture.Get(),
		D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	mCmdList->ClearDepthStencilView(currentDepthStencilDSV(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0, 0xff, 0, nullptr);
	mCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStenceilTexture.Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ));
	
	mCmdList->Close();
	mCmdQueue->ExecuteCommandLists(1, (ID3D12CommandList*const*)mCmdList.GetAddressOf());
	mDxgiSwapChain->Present(0, 0);  

	SwapBackBuffer();	
	FlushCommandQueue();
}
