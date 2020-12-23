#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <DirectXCollision.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>

typedef struct SubMeshGeometry
{
	UINT indexCount;
	UINT startIndexLocation;
	UINT baseIndexLocation;
	
	DirectX::BoundingBox boundBox;
}SubMeshGeometry;


typedef struct MeshGeometry
{
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;

	UINT vertexBufferStride;
	UINT vertexBufferSize;
	DXGI_FORMAT indexBufferFormat;
	UINT indexBufferSize;

	const D3D12_VERTEX_BUFFER_VIEW vertexBufferView()
	{
		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vbv.SizeInBytes = vertexBufferSize;
		vbv.StrideInBytes = vertexBufferStride;
		
		return vbv;
	}

	const D3D12_INDEX_BUFFER_VIEW indexBufferView()
	{
		D3D12_INDEX_BUFFER_VIEW ibv = {};
		ibv.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		ibv.Format = indexBufferFormat;
		ibv.SizeInBytes = indexBufferSize;
		return ibv;
	}

	std::unordered_map<std::string, SubMeshGeometry> drawArgs;
}MeshGeometry;