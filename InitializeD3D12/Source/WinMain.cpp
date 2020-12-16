#include <Windows.h>
#include "GraphicWindow.h"
#include "InitD3DApp.h"
#include <memory>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR lpCmdLine, int nCmdShow)
{
	std::shared_ptr<D3DApp> d3dapp(new InitD3DApp);
	d3dapp->InitializeApp(hInstance);
	d3dapp->Run();

	


	return 0;
}