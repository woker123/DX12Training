#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <DirectXCollision.h>
#include <wrl/client.h>
#include <string>
#include <unordered_map>
#include <vector>

typedef struct SubMeshGeometry
{
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	UINT baseVertexLocation = 0;
	
	DirectX::BoundingBox boundBox;
}SubMeshGeometry;


typedef struct MeshGeometry
{
	std::string Name;

	Microsoft::WRL::ComPtr<ID3D10Blob> VertexBufferCPU;
	Microsoft::WRL::ComPtr<ID3D10Blob> IndexBufferCPU;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader;

	UINT vertexBufferStride = 0;
	UINT vertexBufferSize = 0;
	DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_UNKNOWN;
	UINT indexBufferSize = 0;

	const D3D12_VERTEX_BUFFER_VIEW vertexBufferView()
	{
		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.SizeInBytes = vertexBufferSize;
		vbv.StrideInBytes = vertexBufferStride;
		
		return vbv;
	}

	const D3D12_INDEX_BUFFER_VIEW indexBufferView()
	{
		D3D12_INDEX_BUFFER_VIEW ibv = {};
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = indexBufferFormat;
		ibv.SizeInBytes = indexBufferSize;
		return ibv;
	}

	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}

	std::unordered_map<std::string, SubMeshGeometry> drawArgs;
}MeshGeometry;