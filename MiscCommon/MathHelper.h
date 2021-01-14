//***************************************************************************************
// MathHelper.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Helper math class.
//***************************************************************************************

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <cstdint>

class MathHelper
{
public:
	// Returns random float in [0, 1).
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	// Returns random float in [a, b).
	static float RandF(float a, float b)
	{
		return a + RandF()*(b-a);
	}

    static int Rand(int a, int b)
    {
        return a + rand() % ((b - a) + 1);
    }

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
	 
	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b-a)*t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x); 
	}

	// Returns the polar angle of the point (x,y) in [0, 2*PI).
	static float AngleFromXY(float x, float y);

	static DirectX::XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
	{
		return DirectX::XMVectorSet(
			radius*sinf(phi)*cosf(theta),
			radius*cosf(phi),
			radius*sinf(phi)*sinf(theta),
			1.0f);
	}

    static DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
        DirectX::XMMATRIX A = M;
        A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

        DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
        return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
	}

    static DirectX::XMFLOAT4X4 Identity4x4()
    {
        static DirectX::XMFLOAT4X4 I(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        return I;
    }

	static DirectX::XMFLOAT4X4 Transpose(const DirectX::XMFLOAT4X4& mat)
	{
		DirectX::XMFLOAT4X4 result;
		DirectX::XMStoreFloat4x4(&result, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&mat)));
		return result;
	}

	static DirectX::XMFLOAT3 Cross(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
	{
		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&v1), DirectX::XMLoadFloat3(&v2)));
		return result;
	}

	static DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& v)
	{
		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&v)));
		return result; 
	}

    static DirectX::XMVECTOR RandUnitVec3();
    static DirectX::XMVECTOR RandHemisphereUnitVec3(DirectX::XMVECTOR n);

	static const float Infinity;
	static const float Pi;


};

#define FLOAT3OPFUNCDESC(op)\
inline const DirectX::XMFLOAT3 operator##op(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)\
{\
	using namespace DirectX;\
	XMFLOAT3 result;\
	XMStoreFloat3(&result, XMLoadFloat3(&v1) op XMLoadFloat3(&v2));\
	return result;\
}

FLOAT3OPFUNCDESC(+)
FLOAT3OPFUNCDESC(-)
FLOAT3OPFUNCDESC(*)
FLOAT3OPFUNCDESC(/)