#pragma once

#include "D3DApp.h"
#include "LightFrameResource.h"
#include "RenderItem.h"
#include "D3DShader.h"
#include "Camera.h"
#include "Light.h"

class LightApp : public D3DApp
{
public:
	LightApp() {};
	virtual ~LightApp();

public:
	virtual bool InitializeApp(HINSTANCE hInstance);

private:
	bool BuildRootSignature();
	bool BuildShaders();
	bool BuildPSO();
	bool BuildFrameResources();
	bool BuildMeshGeometry();
	bool BuildRenderItems();
	bool InitCamera();
	void UpdateObjectCB();
	void UpdatePassCB();
	void DrawItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem>& renderItems);

private:
	bool InitLights();
	void UpdateLightCB();

private:
	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual void OnResize(int newWidth, int newHeight) {}
	virtual std::wstring WindowTitile() { return L"Shape App"; }

	virtual void OnKeyboardAxisEvent(KEY_TYPE key);
	virtual void OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState);
	virtual void OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed);

private:
	void CmdListClearRtvAndDsv(ID3D12GraphicsCommandList* cmdList, D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv,float clearColor[4], float depth, unsigned char stencil);
	void CmdListSetVptAndSciRct(ID3D12GraphicsCommandList* cmdList);
	void CmdListOpenRtvAndDsv(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* rtv, ID3D12Resource* dsv);
	void CmdListCloseRtvAndDsv(ID3D12GraphicsCommandList* cmdList, ID3D12Resource* rtv, ID3D12Resource* dsv);

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSig;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mOpaquePSO;

	std::shared_ptr<Camera> mCamera;

	std::shared_ptr<D3DShader> mVSShader;
	std::shared_ptr<D3DShader> mPSShader;

	std::vector<MaterialFrameResource> mFrameResources;
	int mNumFrameResources = 3;
	int mCurFrameResourceIndex = -1;
	MaterialFrameResource* mCurFrameResource = nullptr;
	int mCBVDescriptorSize = 0;

	std::vector<RenderItem> mOpaqueRenderItems;
	
	std::shared_ptr<MeshGeometry> mMeshGeo;
	float mMoveSpeed = 0.01f;
	bool mMouseRightButtonDown = false;

private:
	std::vector<DirectionalLight> mDirectionalLights;
	std::vector<PointLight> mPointLights;
	std::vector<SpotLight> mSpotLights;
};

