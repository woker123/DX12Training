#pragma once

#include "FrameResource.h"
#include "UploadBuffer.h"
#include <memory>
#include "GeometryGenerator.h"


struct WaveFrameResource : public FrameResource
{
	WaveFrameResource(ID3D12Device* device, UINT numPassCB, UINT numObjCB)
		:FrameResource(device, numPassCB, numObjCB) 
	{}
	std::shared_ptr<UploadBuffer<Vertex>> WaveVB;
	Microsoft::WRL::ComPtr<ID3D12Resource> WaveIB;
	Microsoft::WRL::ComPtr<ID3D12Resource> WaveIBUpload;
};
