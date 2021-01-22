#pragma once

#include "FrameResource.h"
#include "Light.h"

struct MaterialFrameResource : public FrameResource
{
	MaterialFrameResource(ID3D12Device* device, UINT numPassCB, UINT numObjCB, UINT numLight)
		:FrameResource(device, numPassCB, numObjCB)
	{
		LightCB.reset(new UploadBuffer<LightConstant>(device, numLight, true));
	}
	virtual ~MaterialFrameResource() {}

	std::shared_ptr<UploadBuffer<LightConstant>> LightCB;
};
