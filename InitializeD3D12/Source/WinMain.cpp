#include <Windows.h>
#include "GraphicWindow.h"



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR lpCmdLine, int nCmdShow)
{
	GraphicWindow window(hInstance, L"window", 50, 50, 800, 800);

	window.showWindow();


	while (window.HandleMessage())
	{




	}


	return 0;
}