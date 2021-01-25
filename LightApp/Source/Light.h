#pragma once
#include <DirectXMath.h>

#define MAX_LIGHT_COUNT 10

struct LightInfo
{
	using float3 = DirectX::XMFLOAT3;

	float3 LightPosition;
	float FalloffStartRadius;
	float3 LightIntensity;
	float FalloffEndRaius;
	float3 LightDirection;
	float FalloffStartAngle;
	float FalloffEndAngle;
	UINT32 LightType;
	UINT32 LightEnable;
	float pad2;
};

struct LightConstant
{
	LightInfo lights[MAX_LIGHT_COUNT];
};

class Light
{
public:
	virtual LightInfo GetLightConstant() = 0;
	virtual ~Light() {}
};

class DirectionalLight : public Light
{
	using float3 = DirectX::XMFLOAT3;
public:
	DirectionalLight(const float3& intensity, const float3& direction)
		:mIntensity(intensity), mDirection(direction)
	{}
	virtual ~DirectionalLight() {}
	
public:
	virtual LightInfo GetLightConstant() override
	{
		LightInfo lightConst = {};
		lightConst.LightIntensity = mIntensity;
		lightConst.LightDirection = mDirection;
		lightConst.LightType = 0;
		lightConst.LightEnable = 1;
		return lightConst;
	}

	float3 GetIntensity() const
	{
		return mIntensity;
	}

	float3 GetDirection() const
	{
		return mDirection;
	}

	void SetIntensity(const float3 intensity)
	{
		mIntensity = intensity;
	}

	void SetDirection(const float3 direction)
	{
		mDirection = direction;
	}

private:
	float3 mIntensity;
	float3 mDirection;
};

class PointLight : public Light
{
	using float3 = DirectX::XMFLOAT3;
public:
	PointLight(const float3& intensity, const float3& position, float falloffStartRadius, float falloffEndRadius)
		:mIntensity(intensity), mPosition(position), mFalloffStartRadius(falloffStartRadius), mFalloffEndRadius(falloffEndRadius)
	{}
	virtual ~PointLight() {}

public:
	virtual LightInfo GetLightConstant() override
	{
		LightInfo lightConst = {};
		lightConst.LightIntensity = mIntensity;
		lightConst.LightPosition = mPosition;
		lightConst.FalloffStartRadius = mFalloffStartRadius;
		lightConst.FalloffEndRaius = mFalloffEndRadius;
		lightConst.LightType = 1;
		lightConst.LightEnable = 1;
		return lightConst;
	}

	float3 GetIntensity() const
	{
		return mIntensity;
	}

	float3 GetPosition() const
	{
		return mPosition;
	}

	float GetFalloffStartRadius() const
	{
		return mFalloffStartRadius;
	}

	float GetFalloffEndRaiuds() const
	{
		return mFalloffEndRadius;
	}

	void SetIntensity(const float3& intensity)
	{
		mIntensity = intensity;
	}

	void SetPosition(const float3& position)
	{
		mPosition = position;
	}

	void SetFalloffStartRadius(float falloffStartRadius)
	{
		mFalloffStartRadius = falloffStartRadius;
	}

	void SetFalloffEndRadius(float falloffEndRadius)
	{
		mFalloffEndRadius = falloffEndRadius;
	}

private:
	float3 mIntensity;
	float3 mPosition;
	float  mFalloffStartRadius;
	float  mFalloffEndRadius;
};

class SpotLight : public Light
{
	using float3 = DirectX::XMFLOAT3;
public:
	SpotLight(const float3& intensity, const float3& position, const float3& direction, 
		float falloffStartRadius, float falloffEndRadius, float falloffStartAngle, float falloffEndAngle)
		:mIntensity(intensity), mPosition(position), mDirection(direction)
		,mFalloffStartRaduis(falloffStartRadius), mFalloffEndRadius(falloffEndRadius)
		,mFalloffStartAngle(falloffStartAngle), mFalloffEndAngle(falloffEndAngle)
	{
	}
	virtual ~SpotLight() {}

public:
	virtual LightInfo GetLightConstant() override
	{
		LightInfo lightConst = {};
		lightConst.LightIntensity = mIntensity;
		lightConst.LightPosition = mPosition;
		lightConst.LightDirection = mDirection;
		lightConst.FalloffStartRadius = mFalloffStartRaduis;
		lightConst.FalloffEndRaius = mFalloffEndRadius;
		lightConst.FalloffStartAngle = mFalloffStartAngle;
		lightConst.FalloffEndAngle = mFalloffEndAngle;
		lightConst.LightType = 2;
		lightConst.LightEnable = 1;
		return lightConst;
	}

	float3 GetIntensity() const
	{
		return mIntensity;
	}

	float3 GetPosition() const
	{
		return mPosition;
	}

	float3 GetDirection() const
	{
		return mDirection;
	}

	float GetFalloffStartRadius() const
	{
		return mFalloffStartRaduis;
	}

	float GetFalloffEndRadius() const
	{
		return mFalloffEndRadius;
	}

	float GetFalloffStartAngle() const
	{
		return mFalloffStartAngle;
	}

	float GetFalloffEndAngle() const
	{
		return mFalloffEndAngle;
	}

	void SetIntensity(const float3& intensity)
	{
		mIntensity = intensity;
	}

	void SetPosition(const float3& position)
	{
		mPosition = position;
	}

	void SetDirection(const float3& direction)
	{
		mDirection = direction;
	}

	void SetFalloffStartRadius(float falloffStartRadius)
	{
		mFalloffStartRaduis = falloffStartRadius;
	}

	void SetFalloffEndRadius(float falloffEndRadius)
	{
		mFalloffEndRadius = falloffEndRadius;
	}

	void SetFalloffStartAngle(float falloffStartAngle)
	{
		mFalloffStartAngle = falloffStartAngle;
	}

	void SetFalloffEndAngle(float falloffEndAngle)
	{
		mFalloffEndAngle = falloffEndAngle;
	}

private:
	float3 mIntensity;
	float3 mPosition;
	float3 mDirection;
	float  mFalloffStartRaduis;
	float  mFalloffEndRadius;
	float  mFalloffStartAngle;
	float  mFalloffEndAngle;
};