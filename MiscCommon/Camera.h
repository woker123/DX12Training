#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera(const DirectX::XMFLOAT3& location, float yaw, float pitch, float fov, float respect, float n = 0.01f, float f = 10000.f)
		:mLocation(location), mYaw(yaw), mPitch(pitch), mFov(fov), mAspect(respect), mNear(n), mFar(f) 
		,mViewMatrix(), mInvViewMatrix(), mProjectMatrix(), mInvProjectMatrix(), mViewProjectMatrix(), mInvViewProjectMatrix()
	{
	}
	~Camera()
	{}

public:
	DirectX::XMFLOAT3 GetLocation() const
	{
		return mLocation;
	}

	float GetYaw() const
	{
		return mYaw;
	}

	float GetPitch() const
	{
		return mPitch;
	}

	float GetNear() const
	{
		return mNear;
	}

	float GetFar() const
	{
		return mFar;
	}

	float GetAspect() const
	{
		return mAspect;
	}

	void SetLocation(const DirectX::XMFLOAT3& location)
	{
		mLocation = location;
	}

	void SetYaw(float yaw)
	{
		mYaw = yaw;
	}

	void SetPitch(float pitch)
	{
		mPitch = pitch;
	}

public:
	void MoveUp(float distance)
	{
		using namespace DirectX;
		XMFLOAT3 upVec = GetUpVector();
		XMVECTOR curLocation = XMLoadFloat3(&mLocation);
		XMVECTOR upVector    = XMLoadFloat3(&upVec);
		XMStoreFloat3(&mLocation, curLocation + upVector * distance);
	}

	void MoveDown(float distance)
	{
		MoveUp(-distance);
	}

	void MoveRight(float distance)
	{
		using namespace DirectX;
		XMFLOAT3 rightVec = GetRightVector();
		XMVECTOR curLocation = XMLoadFloat3(&mLocation);
		XMVECTOR rightVector = XMLoadFloat3(&rightVec);
		XMStoreFloat3(&mLocation, curLocation + rightVector * distance);
	}

	void MoveLeft(float distance)
	{
		MoveRight(-distance);
	}

	void MoveFoward(float distance)
	{
		using namespace DirectX;
		XMFLOAT3 forwardVec = GetForwardVector();
		XMVECTOR curLocation   = XMLoadFloat3(&mLocation);
		XMVECTOR forwardVector = XMLoadFloat3(&forwardVec);
		XMStoreFloat3(&mLocation, curLocation + forwardVector * distance);
	}

	void MoveBack(float distance)
	{
		MoveFoward(-distance);
	}

	void TurnRight(float angle)
	{
		mYaw += angle;
	}

	void TurnLeft(float angle)
	{
		TurnRight(-angle);
	}

	void TurnUp(float angle)
	{
		mPitch += angle;
	}

	void TurnDown(float angle)
	{
		TurnUp(-angle);
	}
public:
	DirectX::XMFLOAT4X4 GetViewMatrix()
	{
		CalculateMatrices();
		return mViewMatrix;
	}

	DirectX::XMFLOAT4X4 GetInvViewMatrix()
	{
		CalculateMatrices();
		return mInvViewMatrix;
	}

	DirectX::XMFLOAT4X4 GetProjectMatrix()
	{
		CalculateMatrices();
		return mProjectMatrix;
	}

	DirectX::XMFLOAT4X4 GetInvProjMatrix()
	{
		CalculateMatrices();
		return mInvProjectMatrix;
	}

	DirectX::XMFLOAT4X4 GetViewProjMatrix()
	{
		CalculateMatrices();
		return mViewProjectMatrix;
	}

	DirectX::XMFLOAT4X4 GetInvViewProjMatrix()
	{
		CalculateMatrices();
		return mInvViewMatrix;
	}

	DirectX::XMFLOAT3 GetForwardVector()
	{
		DirectX::XMFLOAT3 forwardVec;
		DirectX::XMStoreFloat3(&forwardVec, CalculateRotationMatrix().r[2]);
		return forwardVec;
	}

	DirectX::XMFLOAT3 GetRightVector()
	{
		DirectX::XMFLOAT3 rightVec;
		DirectX::XMStoreFloat3(&rightVec, CalculateRotationMatrix().r[0]);
		return rightVec;

	}

	DirectX::XMFLOAT3 GetUpVector()
	{
		DirectX::XMFLOAT3 upVec;
		DirectX::XMStoreFloat3(&upVec, CalculateRotationMatrix().r[1]);
		return upVec;
	}

private:
	DirectX::XMMATRIX XM_CALLCONV CalculateRotationMatrix()
	{
		return DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(mPitch)) * DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(mYaw));
	}

	void CalculateMatrices()
	{
		DirectX::XMMATRIX transMatrixInv = DirectX::XMMatrixTranslation(-mLocation.x, -mLocation.y, -mLocation.z);
		DirectX::XMMATRIX rotaYMatrixInv = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(-mYaw));
		DirectX::XMMATRIX rotaXMatrixInv = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(-mPitch));
		DirectX::XMMATRIX viewMat = transMatrixInv * rotaYMatrixInv * rotaXMatrixInv;
		
		DirectX::XMMATRIX projectMat = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(mFov), mAspect, mNear, mFar);
		DirectX::XMMATRIX viewProjectMat = viewMat * projectMat;
		
		DirectX::XMMATRIX invViewMat = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewMat), viewMat);
		DirectX::XMMATRIX invProjMat = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(projectMat), projectMat);
		DirectX::XMMATRIX invViewProjMat = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(viewProjectMat), viewProjectMat);

		DirectX::XMStoreFloat4x4(&mViewMatrix, viewMat);
		DirectX::XMStoreFloat4x4(&mInvViewMatrix, invViewMat);
		DirectX::XMStoreFloat4x4(&mProjectMatrix, projectMat);
		DirectX::XMStoreFloat4x4(&mInvProjectMatrix, invProjMat);
		DirectX::XMStoreFloat4x4(&mViewProjectMatrix, viewProjectMat);
	}

private:
	DirectX::XMFLOAT3 mLocation;
	float mYaw;
	float mPitch;
	float mFov;
	float mAspect;
	float mNear;
	float mFar;

	DirectX::XMFLOAT4X4 mViewMatrix;
	DirectX::XMFLOAT4X4 mInvViewMatrix;
	DirectX::XMFLOAT4X4 mProjectMatrix;
	DirectX::XMFLOAT4X4	mInvProjectMatrix;
	DirectX::XMFLOAT4X4 mViewProjectMatrix;
	DirectX::XMFLOAT4X4 mInvViewProjectMatrix;
};
