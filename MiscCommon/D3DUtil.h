#pragma once

#include <d3d12.h>
#include <wrl/client.h>


class D3DUtil
{
public:
	static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	static UINT CalcConstantBufferByteSize(UINT byteSize);

	static D3D12_GRAPHICS_PIPELINE_STATE_DESC CreateDefaultPSO();

};


