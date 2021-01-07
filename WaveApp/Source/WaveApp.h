#pragma once
#include "D3DApp.h"
#include "Camera.h"
#include "D3DShader.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "MeshGeometry.h"
#include "GeometryGenerator.h"
#include "UploadBuffer.h"
#include <memory>
#include <vector>


class WaveApp : public D3DApp
{
public:
	WaveApp() {}
	virtual ~WaveApp();

public:
	virtual bool InitializeApp(HINSTANCE hInstance) override;

private:
	virtual void OnKeyboardAxisEvent(KEY_TYPE key);
	virtual void OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState) override;
	virtual void OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed) override;

	virtual void Update(float deltaTime) override;
	virtual void Draw() override;
	virtual std::wstring WindowTitile() override { return L"Wave App"; }

private:
	bool InitCamere();
	bool InitMeshGeo();
	bool InitRenderItems();
	bool InitShaders();
	bool InitFrameResources();
	bool InitRootSignature();
	bool InitPSO();

	void UpdatePassCB(FrameResource* frameResource);
	void UpdateObjectCB(FrameResource* frameResource);
	void DrawRenderItems();

private:
	std::shared_ptr<Camera> mCamera;
	std::shared_ptr<MeshGeometry> mMeshGeo;
	std::shared_ptr<D3DShader> mVS;
	std::shared_ptr<D3DShader> mPS;
	std::vector<RenderItem> mRenderItems;
	std::vector<FrameResource> mFrameResources;

	UINT mNumFrameResource = 5;
	UINT mCurFrameResourceIndex = 0;
	float mMoveSpeed = 0.01f;
	bool mMouseRightButtonDown = false;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO;

};

