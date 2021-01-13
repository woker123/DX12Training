#include "GeometryGenerator.h"

MeshData GeometryGenerator::GenerateBox(float xSize, float ySize, float zSize, int xSlice, int ySlice, int zSlice)
{
	MeshData boxMesh = {};
	float halfX = xSize * 0.5f;
	float halfY = ySize * 0.5f;
	float halfZ = zSize * 0.5f;
	DirectX::XMFLOAT2 tex[4] = { {0.f, 0.f}, {1.f, 0.f}, {1.f, 1.f}, {0.f, 1.f} };

	//negative-y
	Vertex vert[4] = {};
	vert[0].Position = {halfX, -halfY, halfZ};
	vert[1].Position = {-halfX, -halfY, halfZ};
	vert[2].Position = {-halfX, -halfY, -halfZ};
	vert[3].Position = {halfX, -halfY, -halfZ};
	for (int i = 0; i < 4; ++i)
	{
		vert[i].Normal = { 0.f, -1.f, 0.f };
		vert[i].Tangent = { -1.f, 0.f, 0.f };
		vert[i].Texture = tex[i];
		boxMesh.Vertices.push_back(vert[i]);
	}

	//positive-y
	vert[0].Position = { -halfX, halfY, halfZ };
	vert[1].Position = { halfX, halfY, halfZ };
	vert[2].Position = { halfX, halfY, -halfZ };
	vert[3].Position = { -halfX, halfY, -halfZ };
	for (int i = 0; i < 4; ++i)
	{
		vert[i].Normal = { 0.f, 1.f, 0.f };
		vert[i].Tangent = { 1.f, 0.f, 0.f };
		vert[i].Texture = tex[i];
		boxMesh.Vertices.push_back(vert[i]);
	}

	//negative-x
	vert[0].Position = { -halfX, -halfY, halfZ };
	vert[1].Position = { -halfX, halfY, halfZ };
	vert[2].Position = { -halfX, halfY, -halfZ };
	vert[3].Position = { -halfX, -halfY, -halfZ };
	for (int i = 0; i < 4; ++i)
	{
		vert[i].Normal = { -1.f, 0.f, 0.f };
		vert[i].Tangent = { 0.f, 1.f, 0.f };
		vert[i].Texture = tex[i];
		boxMesh.Vertices.push_back(vert[i]);
	}

	//positive-x
	vert[0].Position = { halfX, halfY, -halfZ };
	vert[1].Position = { halfX, halfY, halfZ };
	vert[2].Position = { halfX, -halfY, halfZ };
	vert[3].Position = { halfX, -halfY, -halfZ };
	for (int i = 0; i < 4; ++i)
	{
		vert[i].Normal = { 1.f, 0.f, 0.f };
		vert[i].Tangent = { 0.f, 0.f, 1.f };
		vert[i].Texture = tex[i];
		boxMesh.Vertices.push_back(vert[i]);
	}

	//negative-z
	vert[0].Position = { -halfX, halfY, -halfZ };
	vert[1].Position = { halfX, halfY, -halfZ };
	vert[2].Position = { halfX, -halfY, -halfZ };
	vert[3].Position = { -halfX, -halfY, -halfZ };
	for (int i = 0; i < 4; ++i)
	{
		vert[i].Normal = { 0.f, 0.f, -1.f };
		vert[i].Tangent = { 1.f, 0.f, 0.f };
		vert[i].Texture = tex[i];
		boxMesh.Vertices.push_back(vert[i]);
	}

	//positive-z
	vert[0].Position = { halfX, halfY, halfZ };
	vert[1].Position = { -halfX, halfY, halfZ };
	vert[2].Position = { -halfX, -halfY, halfZ };
	vert[3].Position = { halfX, -halfY, halfZ };
	for (int i = 0; i < 4; ++i)
	{
		vert[i].Normal = {0.f, 0.f, 1.f};
		vert[i].Tangent = {-1.f, 0.f, 0.f};
		vert[i].Texture = tex[i];
		boxMesh.Vertices.push_back(vert[i]);
	}

	//generate indices
	for (int i = 0; i < 6; ++i)
	{
		boxMesh.Indices32.push_back(4 * i);
		boxMesh.Indices32.push_back(4 * i + 1);
		boxMesh.Indices32.push_back(4 * i + 3);
		boxMesh.Indices32.push_back(4 * i + 3);
		boxMesh.Indices32.push_back(4 * i + 1);
		boxMesh.Indices32.push_back(4 * i + 2);
	}
	return boxMesh;
}

void GeometryGenerator::BuildCylinderLateral(MeshData& meshData, float bottomRadius, float topRadius, float height, int sliceCount, int stackCount)
{
	using namespace DirectX;
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
		for (int j = 0; j < sliceCount + 1; ++j)
		{
			theta = j * XM_2PI / sliceCount;
			c = std::cos(theta);
			s = std::sin(theta);

			XMFLOAT3 pos = { r * c, y , r * s };
			XMFLOAT2 tex = { 1.f / sliceCount * j, 1.f - 1.f / stackCount * i };
			XMFLOAT3 tan = { -s, 0, c };
			XMFLOAT3 bitan = { (topRadius - bottomRadius) * c, height, (topRadius - bottomRadius) * s };
			XMVECTOR bitanv = XMLoadFloat3(&bitan);
			bitanv = XMVector3Normalize(bitanv);
			XMVECTOR tanv = XMLoadFloat3(&tan);
			XMVECTOR normv = XMVector3Cross(bitanv, tanv);
			XMFLOAT3 norm;
			XMStoreFloat3(&norm, normv);
			meshData.Vertices.push_back({ pos, norm, tan, tex });
		}
		y += deltaY;
		r += deltaR;
	}

	//build lateral indices
	for (int i = 0; i < stackCount; ++i)
	{
		for (int j = 0; j < sliceCount; ++j)
		{
			meshData.Indices32.push_back(i * (sliceCount + 1) + j);
			meshData.Indices32.push_back((i + 1) * (sliceCount + 1) + j);
			meshData.Indices32.push_back((i + 1) * (sliceCount + 1) + j + 1);

			meshData.Indices32.push_back(i * (sliceCount + 1) + j);
			meshData.Indices32.push_back((i + 1) * (sliceCount + 1) + j + 1);
			meshData.Indices32.push_back(i * (sliceCount + 1) + j + 1);
		}
	}
}

void GeometryGenerator::BuildCylinderTopCap(MeshData& meshData, float bottomRadius, float topRadius, float height, int sliceCount, int stackCount)
{
	//top cap vertices
	uint32 baseVertexLocation = (uint32)meshData.Vertices.size();
	float halfHeight = height * 0.5f;
	float theta = DirectX::XM_2PI;
	float deltaTheta = DirectX::XM_2PI / (float)sliceCount;
	meshData.Vertices.push_back(Vertex({ 0.f, halfHeight, 0.f }, { 0.f, 1.f, 0.f }, { 1.f, 0.f, 0.f }, { 0.5f, 0.5f }));
	for (int i = 0; i < sliceCount + 1; ++i)
	{
		Vertex vert = {};
		vert.Position = { topRadius * std::cos(theta), halfHeight, topRadius * std::sin(theta) };
		vert.Normal = { 0.f, 1.f, 0.f };
		vert.Tangent = { 1.f, 0.f, 0.f };
		vert.Texture = { vert.Position.x / (2.f * topRadius) + 0.5F, 0.5F - vert.Position.z / (2.f * topRadius) };
		meshData.Vertices.push_back(vert);
		theta -= deltaTheta;
	}

	//top cap indices 
	for (int i = 0; i < sliceCount; ++i)
	{
		meshData.Indices32.push_back(baseVertexLocation + 0);
		meshData.Indices32.push_back(baseVertexLocation + i + 1);
		meshData.Indices32.push_back(baseVertexLocation + i + 2);
	}
}

void GeometryGenerator::BuildCylinderBottomCap(MeshData& meshData, float bottomRadius, float topRadius, float height, int sliceCount, int stackCount)
{
	//bottom cap vertices
	float halfHeight = 0.5f * height;
	uint32 baseVertexLocation = (uint32)meshData.Vertices.size();
	float theta = 0.f;
	float deltaTheta = DirectX::XM_2PI / (float)sliceCount;
	meshData.Vertices.push_back(Vertex({ 0.f, -halfHeight, 0.f }, { 0.f, -1.f, 0.f }, { 1.f, 0.f, 0.f }, { 0.5f, 0.5f }));
	for (int i = 0; i < sliceCount + 1; ++i)
	{
		Vertex vert = {};
		vert.Position = { bottomRadius * std::cos(theta), -halfHeight, bottomRadius * std::sin(theta) };
		vert.Normal = { 0.f, -1.f, 0.f };
		vert.Tangent = { 1.f, 0.f, 0.f };
		vert.Texture = { vert.Position.x / (2.f * bottomRadius) + 0.5f, 0.5f - vert.Position.z / (2.f * bottomRadius) };
		meshData.Vertices.push_back(vert);
		theta += deltaTheta;
	}

	//bottom cap indices
	for (int i = 0; i < sliceCount; ++i)
	{
		meshData.Indices32.push_back(baseVertexLocation + 0);
		meshData.Indices32.push_back(baseVertexLocation + i + 1);
		meshData.Indices32.push_back(baseVertexLocation + i + 2);
	}

}

MeshData GeometryGenerator::GenerateCylinder(float bottomRadius, float topRadius, float height, int sliceCount, int stackCount)
{
	using namespace DirectX;
	MeshData cylinderMesh;
	
	BuildCylinderLateral(cylinderMesh, bottomRadius, topRadius, height, sliceCount, stackCount);
	BuildCylinderTopCap(cylinderMesh, bottomRadius, topRadius, height, sliceCount, stackCount);
	BuildCylinderBottomCap(cylinderMesh, bottomRadius, topRadius, height, sliceCount, stackCount);

	return cylinderMesh;
}

MeshData GeometryGenerator::GenerateSphere(float radius, int sliceCount, int stackCount)
{
	using namespace DirectX;
	MeshData sphereMesh = {};
	float phi = 0.f;
	float theta = 0.f;
	float deltaPhi = XM_2PI / (float)sliceCount;
	float deltaTheta = XM_PI / (float)stackCount;

	//generate vertices
	//first vertex
	sphereMesh.Vertices.push_back(Vertex({0.f, radius, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}, {0.5f, 0.0f}));
	theta = XM_PI / stackCount;
	for (int stackIndex = 1; stackIndex < stackCount; ++stackIndex)
	{
		phi = 0.f;
		for (int sliceIndex = 0; sliceIndex < sliceCount + 1; ++sliceIndex)
		{
			Vertex vert = {};
			vert.Position = {radius * std::sin(theta) * std::cos(phi), radius * std::cos(theta), radius * std::sin(theta) * std::sin(phi)};
			vert.Normal = {vert.Position.x / radius, vert.Position.y / radius, vert.Position.z / radius};
			vert.Tangent = {-std::sin(phi), 0.f, std::cos(phi)};
			vert.Texture = {(float)sliceIndex / sliceCount, (float)stackIndex / stackCount};
			sphereMesh.Vertices.push_back(std::move(vert));
			phi -= deltaPhi;
		}
		theta += deltaTheta;
	}
	//last vertex
	sphereMesh.Vertices.push_back(Vertex({0.f, -radius, 0.f}, {0.f, -1.f, 0.f}, {0.f, 0.f, 1.f}, {0.5f, 1.f}));

	//generate indices
	//top point with adjoint vertices
	uint32 topPointIndex = 0;
	uint32 bottomPointIndex = (uint32)sphereMesh.Vertices.size() - 1;
	for (int i = 0; i < sliceCount; ++i)
	{
		sphereMesh.Indices32.push_back(topPointIndex);
		sphereMesh.Indices32.push_back(i + 1);
		sphereMesh.Indices32.push_back(i + 2);
	}

	//lateral indices
	for (int i = 1; i < stackCount - 1; ++i)
	{
		for (int j = 0; j < sliceCount; ++j)
		{
			sphereMesh.Indices32.push_back((i - 1) * (sliceCount + 1) + j + 1);
			sphereMesh.Indices32.push_back(i * (sliceCount + 1) + j + 1);
			sphereMesh.Indices32.push_back((i - 1) * (sliceCount + 1) + j + 2);
			
			sphereMesh.Indices32.push_back(i * (sliceCount + 1) + j + 1);
			sphereMesh.Indices32.push_back(i * (sliceCount + 1) + j + 2);
			sphereMesh.Indices32.push_back((i - 1) * (sliceCount + 1) + j + 2);
		} 
	}
	//bottom point with adjoint vertices
	for (int i = 0; i < sliceCount + 1; ++i)
	{
		sphereMesh.Indices32.push_back(bottomPointIndex);
		sphereMesh.Indices32.push_back(bottomPointIndex - i);
		sphereMesh.Indices32.push_back(bottomPointIndex - i - 1);
	}
	return sphereMesh;
}

MeshData GeometryGenerator::GenerateLandscape(float width, float height, int wNumGrid, int hNumGrid)
{
	MeshData meshData = {};
	BuildGrid(meshData, width, height, wNumGrid, hNumGrid);
	for (auto& vertex : meshData.Vertices)
	{
		vertex.Position.y = 0.2f * CalcHeight(5 * vertex.Position.x, 5 * vertex.Position.z);
	}

	return meshData;
}

MeshData GeometryGenerator::GenerateWave(float width, float height, int wNumGrid, int hNumGrid)
{
	MeshData meshData = {};
	BuildGrid(meshData, width, height, wNumGrid, hNumGrid);

	return meshData;
}

void GeometryGenerator::BuildGrid(MeshData& meshData, float width, float height, int wNumGride, int hNumGride)
{
	int xNumLine = wNumGride + 1;
	int zNumLine = hNumGride + 1;
	float deltaX = width / (float)wNumGride;
	float deltaZ = height / (float)hNumGride;
	float x = -width * 0.5f;
	float z = height * 0.5f;

	//generate vertices
	meshData.Vertices.reserve((size_t)xNumLine * (size_t)zNumLine);
	for (int i = 0; i < zNumLine; ++i)
	{
		x = -width * 0.5f;
		for (int j = 0; j < xNumLine; ++j)
		{
			meshData.Vertices.push_back({ {x, 0, z}, {0.f, 1.f, 0.f}, {1.f, 0.f, 0.f}, {(float)i / (float)hNumGride, (float)j / (float)wNumGride} });
			x += deltaX;
		}
		z -= deltaZ;
	}

	//generate indices
	meshData.Indices32.reserve((size_t)wNumGride * (size_t)hNumGride * 2);
	for (int i = 0; i < hNumGride; ++i)
	{
		for (int j = 0; j < wNumGride; ++j)
		{
			meshData.Indices32.push_back(i * xNumLine + j);
			meshData.Indices32.push_back(i * xNumLine + j + 1);
			meshData.Indices32.push_back((i + 1) * xNumLine + j);

			meshData.Indices32.push_back((i + 1) * xNumLine + j);
			meshData.Indices32.push_back(i * xNumLine + j + 1);
			meshData.Indices32.push_back((i + 1) * xNumLine + j + 1);
		}
	}
}

float GeometryGenerator::CalcHeight(float x, float z)
{
	return 0.3f * (z * std::sinf(0.1f * x) + x * cosf(0.1f * z));
}
