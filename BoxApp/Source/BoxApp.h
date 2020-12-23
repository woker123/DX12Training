#pragma once

#include "D3DApp.h"
#include "D3D12Shader.h"
#include "UploadBuffer.h"
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "MeshGeometry.h"
#include "Camera.h"

typedef struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
}Vertex;

class BoxApp : public D3DApp
{
public:
	BoxApp():mMVPMatrix() {}
	virtual ~BoxApp() {}

public:
	virtual bool InitializeApp(HINSTANCE hInstance) override;
	virtual std::wstring WindowTitile() override { return L"Box App"; }

private:
	//input 
	virtual void OnKeyboardActionEvent(KEY_TYPE key, PRESS_STATE pState) override;
	virtual void OnKeyboardAxisEvent(KEY_TYPE key) override;
	virtual void OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState) override;
	virtual void OnMouseButtonAxisEvent(MOUSE_BUTTON_TYPE mouseButton) override;
	virtual void OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed) override;

private:
	bool BuildCBVHeap();
	bool BuildCBuffer();
	bool BuildRootSignature();;
	bool BuildShaderAndInputLayout();
	bool BuildPSO();
	bool BuildMesh();
	virtual void Update(float deltaTime) override;
	virtual void Draw() override;


private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCBVHeap;
	std::shared_ptr<UploadBuffer<DirectX::XMFLOAT4X4>> mConstBuffer;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
	Microsoft::WRL::ComPtr<ID3D10Blob> mVSBlob;
	Microsoft::WRL::ComPtr<ID3D10Blob> mPSBlob;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO;
	std::shared_ptr<MeshGeometry> mDrawMesh;
	Microsoft::WRL::ComPtr<ID3D12Resource> mVertexBufferUpload;
	Microsoft::WRL::ComPtr<ID3D12Resource> mIndexBufferUpload;

	std::shared_ptr<Camera> mCamera;
	DirectX::XMFLOAT4X4 mMVPMatrix;
	bool mMouseRightButtonDown = false;
};