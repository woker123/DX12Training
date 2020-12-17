#pragma once

#include "D3DApp.h"


class InitD3DApp : public D3DApp
{
public:
	InitD3DApp() {}
	virtual ~InitD3DApp() {}
	
private:
	virtual void Draw() override;

private:
	void ClearRTVAndDSV(ID3D12GraphicsCommandList* cmdList,float color[4], float depth, unsigned char stencil);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexBuffer;

};