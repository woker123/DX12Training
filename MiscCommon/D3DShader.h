#pragma once

#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <map>
#include <assert.h>

#pragma comment(lib, "d3dcompiler.lib")

class D3DShader
{
public:
	D3DShader(LPCWSTR fileName, LPCSTR target, D3D_SHADER_MACRO* macros = nullptr)
	{
		bool D3D12Shader_Construct = InitializeShader(fileName, target, macros);
		assert(D3D12Shader_Construct);
	}
	~D3DShader() {}

public:
	ID3D10Blob* GetBlob()
	{
		return mShaderBlob.Get();
	}

private:
	bool InitializeShader(LPCWSTR shaderFile, LPCSTR target, D3D_SHADER_MACRO* macros)
	{
		UINT compileFlag = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		HRESULT result = D3DCompileFromFile(shaderFile, macros, nullptr, "main", target, compileFlag, 0, &mShaderBlob, nullptr);
		return SUCCEEDED(result);
	}

private:
	Microsoft::WRL::ComPtr<ID3D10Blob> mShaderBlob;

};
