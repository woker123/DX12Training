#pragma once

#include "FrameResource.h"
#include "Light.h"

struct LightFrameResource : public FrameResource
{
	LightFrameResource(ID3D12Device* device, UINT numPassCB, UINT numObjCB)
		:FrameResource(device, numPassCB, numObjCB)
	{
		LightCB.reset(new UploadBuffer<LightConstant>(device, 1, true));
	}
	virtual ~LightFrameResource() {}

	std::shared_ptr<UploadBuffer<LightConstant>> LightCB;
};
