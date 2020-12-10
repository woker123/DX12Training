#include "GraphicWindow.h"
#include <assert.h>

LRESULT GraphicWindow::WinProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (nMsg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		result = 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		result = 0;
		break;
	default:
		result = DefWindowProc(hwnd, nMsg, wParam, lParam);
		break;
	}

	return result;
}

GraphicWindow::GraphicWindow(HINSTANCE hInstance,LPCTSTR title, int posX, int posY, int width, int height)
	:m_posX(posX), m_posY(posY), m_width(width), m_height(height)
{
	bool GraphicWindow_Init = initGraphicWindow(hInstance, title);
	assert(GraphicWindow_Init);
}

GraphicWindow::~GraphicWindow()
{
	destroyGraphicWindow();
}

bool GraphicWindow::HandleMessage()
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message != WM_QUIT)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool GraphicWindow::initGraphicWindow(HINSTANCE hInstance, LPCTSTR title)
{
	LPCTSTR WCNAME = L"G_WINDOW";
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = GraphicWindow::WinProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = WCNAME;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hbrBackground = CreateSolidBrush(COLOR_BACKGROUND);

	RegisterClassEx(&wcex);

	DWORD wndStyle = WS_OVERLAPPEDWINDOW;
	RECT wndSize = {m_posX, m_posY, m_posX + m_width, m_posY + m_height};
	AdjustWindowRect(&wndSize, WS_OVERLAPPEDWINDOW, false);
	m_hwnd = CreateWindowEx(
		0,
		WCNAME,
		title,
		wndStyle,
		wndSize.left,
		wndSize.top,
		wndSize.right - wndSize.left,
		wndSize.bottom - wndSize.top,
		NULL, 
		NULL,
		hInstance,
		NULL
		);

	return (bool)m_hwnd;
}

void GraphicWindow::destroyGraphicWindow()
{
	DestroyWindow(m_hwnd);
}
