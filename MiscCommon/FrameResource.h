#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "UploadBuffer.h"
#include <DirectXMath.h>
#include <assert.h>

struct PassConstant
{
	DirectX::XMFLOAT4X4 View;
	DirectX::XMFLOAT4X4 InvView;
	DirectX::XMFLOAT4X4 Proj;
	DirectX::XMFLOAT4X4 InvProj;
	DirectX::XMFLOAT4X4 ViewProj;
	DirectX::XMFLOAT4X4 InvViewProj;
	DirectX::XMFLOAT3 EyePosW;
	FLOAT CBPerObjectPad1 = 0.f;
	DirectX::XMFLOAT2 RenderTargetSize;
	DirectX::XMFLOAT2 InvRenderTargetSize;
	FLOAT NearZ = 0.f;
	FLOAT FarZ = 0.f;
	FLOAT TotalTime = 0.f;
	FLOAT DeltaTime = 0.f;
};

struct ObjectConstant
{
	DirectX::XMFLOAT4X4 Model;
	DirectX::XMFLOAT4X4 InvModel;
};

struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT numPassCB, UINT numObjCB)
	{
		HRESULT result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CmdAlloc));
		assert(SUCCEEDED(result) && "Command allocator creation failed in frame resource");
		PassCB = std::make_shared<UploadBuffer<PassConstant>>(device, numPassCB, true);
		ObjectCB = std::make_shared<UploadBuffer<ObjectConstant>>(device, numObjCB, true);
	}
	virtual ~FrameResource() {}

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdAlloc;
	std::shared_ptr<UploadBuffer<PassConstant>> PassCB;
	std::shared_ptr<UploadBuffer<ObjectConstant>> ObjectCB;

	UINT64 FenceValue = 0;
};

