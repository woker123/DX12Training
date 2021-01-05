#pragma once

#include <vector>
#include <DirectXMath.h>
#include <cmath>

using uint32 = std::uint32_t;
using uint16 = std::uint16_t;

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Tangent;
	DirectX::XMFLOAT2 Texture;

	Vertex()
		:Position(0.f, 0.f, 0.f)
		,Normal(0.f, 0.f, 0.f)
		,Tangent(0.f, 0.f, 0.f)
		,Texture(0.f, 0.f)
	{}

	Vertex(const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT3& normal,
		const DirectX::XMFLOAT3& tangent,
		const DirectX::XMFLOAT2& texture)
		:Position(position)
		,Normal(normal)
		,Tangent(tangent)
		,Texture(texture)
	{}

	Vertex(float positionX, float positionY, float positionZ,
		float normalX, float normalY, float normalZ,
		float tangentX, float tangentY, float tangentZ,
		float textureU, float textureV)
		:Position(positionX, positionY, positionZ)
		,Normal(normalX, normalY, normalZ)
		,Tangent(tangentX, tangentY, tangentZ)
		,Texture(textureU, textureV)
	{}
};

struct MeshData
{
	std::vector<Vertex> Vertices;
	std::vector<uint32> Indices32;

	std::vector<uint16>& GetIndices16()
	{
		if (mIndices16.empty())
		{
			mIndices16.resize(Indices32.size());
			for (int i = 0; i < mIndices16.size(); ++i)
			{
				mIndices16[i] = (uint16)Indices32[i];
			}
		}
		return mIndices16;
	}

private:
	std::vector<uint16> mIndices16;

};


class GeometryGenerator
{
public:
	static MeshData GenerateBox(float xSize, float ySize, float zSize, int xSlice = 8, int ySlice = 8, int zSlice = 8);
	
	static MeshData GenerateCylinder(float bottomRadius, float topRadius, float height, int sliceCount, int stackCount);	

	static MeshData GenerateSphere(float radius, int sliceCount, int stackCount);

private:
	static void BuildCylinderLateral(MeshData& meshData, float bottomRadius, float topRadius, float height, int sliceCount, int stackCount);

	static void BuildCylinderTopCap(MeshData& meshData, float bottomRadius, float topRadius, float height, int sliceCount, int stackCount);

	static void BuildCylinderBottomCap(MeshData& meshData, float bottomRadius, float topRadius, float height, int sliceCount, int stackCount);

};


