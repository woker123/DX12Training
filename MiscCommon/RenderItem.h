#pragma once
#include <DirectXMath.h>
#include "MeshGeometry.h"

struct RenderItem
{
	DirectX::XMFLOAT4X4 ModelMat = DirectX::XMFLOAT4X4();
	
	UINT NumFramesDirty = 0;

	UINT ObjCBIndex = -1;

	MeshGeometry* GeoMesh = nullptr;

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

	UINT DrawIndexCount = 0;
	UINT DrawStartIndex = 0;
	UINT BaseVertexLocation = 0;
};
