#include <Windows.h>
#include "LightApp.h"
#include <memory>

INT WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	std::shared_ptr<D3DApp> app(new LightApp);
	app->InitializeApp(hInstance);
	app->Run();

	return 0;
}

