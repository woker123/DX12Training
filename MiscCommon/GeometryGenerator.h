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
	static MeshData GenerateBox(float length)
	{
		MeshData boxMesh;
		float halfLen = 0.5f * length;

		boxMesh.Vertices =
		{
			Vertex({-halfLen, -halfLen, -halfLen}, {}, {}, {}),
			Vertex({-halfLen, +halfLen, -halfLen}, {}, {}, {}),
			Vertex({+halfLen, +halfLen, -halfLen}, {}, {}, {}),
			Vertex({+halfLen, -halfLen, -halfLen}, {}, {}, {}),
			Vertex({-halfLen, -halfLen, +halfLen}, {}, {}, {}),
			Vertex({-halfLen, +halfLen, +halfLen}, {}, {}, {}),
			Vertex({+halfLen, +halfLen, +halfLen}, {}, {}, {}),
			Vertex({+halfLen, -halfLen, +halfLen}, {}, {}, {})
		};

		boxMesh.Indices32 =
		{
			0, 1, 2,
			0, 2, 3,

			4, 6, 5,
			4, 7, 6,

			4, 5, 1,
			4, 1, 0,

			3, 2, 6,
			3, 6, 7,

			1, 5, 6,
			1, 6, 2,

			4, 0, 3,
			4, 3, 7
		};
		return boxMesh;
	}

	static MeshData GenerateCylinder(float bottomRadius, float topRadius, float height, int sliceCount, int stackCount)
	{
		using namespace DirectX;
		MeshData cylinderMesh;
		float y = -height * 0.5f;
		float deltaY = height / stackCount;
		float r = bottomRadius;
		float deltaR = (topRadius - bottomRadius) / stackCount;
		float theta = 0.f;
		float c = std::cos(theta);
		float s = std::sin(theta);
		
		//build lateral vertices
		for (int i = 0; i < stackCount + 1; ++i)
		{
			theta = 0.f;
			for (int j = 0; j < sliceCount; ++j)
			{
				theta = j * XM_2PI / sliceCount;
				c = std::cosf(theta);
				s = std::sinf(theta);

				XMFLOAT3 pos = {r * c, y , r * s};
				XMFLOAT2 tex = {1.f / (sliceCount - 1) * j, 1.f - 1.f / stackCount * i};
				XMFLOAT3 tan = {-s, 0, c};
				XMFLOAT3 bitan = {(topRadius - bottomRadius) * c, height, (topRadius - bottomRadius) * s};
				XMVECTOR bitanv = XMLoadFloat3(&bitan);
				bitanv = XMVector3Normalize(bitanv);
				XMVECTOR tanv = XMLoadFloat3(&tan);
				XMVECTOR normv = XMVector3Cross(bitanv, tanv);
				XMFLOAT3 norm;
				XMStoreFloat3(&norm, normv);
				cylinderMesh.Vertices.push_back({pos, norm, tan, tex});
			}
			y += deltaY;
			r += deltaR;
		}

		//build lateral indices
		for (int i = 0; i < stackCount; ++i)
		{
			for (int j = 0; j < sliceCount - 1; ++j)
			{
				cylinderMesh.Indices32.push_back(i * sliceCount + j);
				cylinderMesh.Indices32.push_back((i + 1) * sliceCount + j);
				cylinderMesh.Indices32.push_back((i + 1) * sliceCount + j + 1);

				cylinderMesh.Indices32.push_back(i * sliceCount + j);
				cylinderMesh.Indices32.push_back((i + 1) * sliceCount + j + 1);
				cylinderMesh.Indices32.push_back(i * sliceCount + j + 1);
			}
			uint32 s = sliceCount * (i + 1);
			cylinderMesh.Indices32.push_back(s - 1);
			cylinderMesh.Indices32.push_back(s + sliceCount - 1);
			cylinderMesh.Indices32.push_back(s);

			cylinderMesh.Indices32.push_back(s - 1);
			cylinderMesh.Indices32.push_back(s);
			cylinderMesh.Indices32.push_back(s - sliceCount);
		}
	



		return cylinderMesh;
	}

};


