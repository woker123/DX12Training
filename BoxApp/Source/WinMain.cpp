#include <Windows.h>
#include "BoxApp.h"
#include <memory>
#include <assert.h>

UINT WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	std::shared_ptr<BoxApp> boxApp(new BoxApp);
	bool initOK = boxApp->InitializeApp(hInstance);
	assert(initOK);
	boxApp->Run();

	return 0;
}

