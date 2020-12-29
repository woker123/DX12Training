#pragma once

#include <d3d12.h>
#include "d3dx12.h"
#include <wrl/client.h>
#include <assert.h>

template<class T>
class UploadBuffer
{
public:
	UploadBuffer(ID3D12Device* device, int elementCount, bool isConstantBuffer)
	:mDevice(device) ,mElementCount(elementCount), mIsConstantBuffer(isConstantBuffer)
	,mElementByteSize(0) ,mMappedData(nullptr)
	{
		InitializeUploadBuffer();
	}
	~UploadBuffer() 
	{
		mUploadBuffer->Unmap(0, nullptr);
		mUploadBuffer = nullptr;
	}

public:
	ID3D12Resource* GetD3DResource()
	{
		return mUploadBuffer.Get();
	}
	
	void CopyData(int elementIndex, const T& element)
	{
		T* dest = (T*)((char*)mMappedData + (UINT64)elementIndex * (UINT64)mElementByteSize);
		*dest = element;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetElementGUPVirtualAddress(int eleIndex)
	{
		return mUploadBuffer->GetGPUVirtualAddress() + D3D12_GPU_VIRTUAL_ADDRESS((UINT64)eleIndex * (UINT64)mElementByteSize);
	}

	UINT GetElementSizeByte()
	{
		return mElementByteSize;
	}

private:
	void InitializeUploadBuffer()
	{
		mElementByteSize = mIsConstantBuffer ? CalcConstantBufferByteSize(sizeof(T)) : sizeof(T);
		UINT bufferByteSize = mElementByteSize * mElementCount;
		HRESULT result = mDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferByteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ, 
			nullptr,
			IID_PPV_ARGS(&mUploadBuffer));
		assert(SUCCEEDED(result));

		mUploadBuffer->Map(0, nullptr, (void**)&mMappedData);
	}

	UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 255) & ~255;
	}

private:
	ID3D12Device* mDevice;
	Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
	bool mIsConstantBuffer;
	int mElementCount;
	UINT mElementByteSize;
	T* mMappedData;
};