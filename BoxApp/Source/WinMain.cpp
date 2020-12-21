#include <Windows.h>
#include "BoxApp.h"
#include <memory>

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, PWSTR lpCmdLine, INT nCmdShow)
{
	std::shared_ptr<BoxApp> boxApp(new BoxApp);
	boxApp->InitializeApp(hInstance);
	boxApp->Run();

	return 0;
}

