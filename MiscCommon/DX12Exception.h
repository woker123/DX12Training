#pragma once

#include <Windows.h>
#include <comdef.h>
#include <string>
#include <codecvt>
#include <locale>

class DxException
{
public:
	DxException() = default;

	DxException(HRESULT hResult, const std::wstring& functionName, const std::wstring& fileName, int lineNumber)
		:mErrorCode(hResult), mFunctionName(functionName), mFileName(fileName), mLineNumber(lineNumber) {}

	std::wstring ToString() const
	{
		_com_error error(mErrorCode);
		return error.ErrorMessage();
	}

	HRESULT mErrorCode = S_OK;
	std::wstring mFunctionName;
	std::wstring mFileName;
	int mLineNumber = -1;
};


#if defined(_DEBUG) || defined(DEBUG)

#ifndef ThrowIfFailed()
#define ThrowIfFailed(func)\
HRESULT result = func;\
if(FAILED(result)){\
std::wstring fileName = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(__FILE__);\
throw DxException(result, L#func, fileName, __LINE__);}
#endif // !ThrowIfFailed()

#else
#define ThrowIfFailed(func) func;

#endif