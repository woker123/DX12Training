#pragma once

#include <Windows.h>

class GraphicWindow
{
private:
	/**window proc function, can only be setted by this class*/
	static LRESULT CALLBACK WinProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

public:
	GraphicWindow(HINSTANCE hInstance, LPCTSTR title, int posX, int posY, int width, int height);
	~GraphicWindow();

	GraphicWindow(const GraphicWindow&) = delete;
	GraphicWindow& operator=(const GraphicWindow&) = delete;

public:
	/**get x position of this window*/
	int getPosX() const
	{
		return m_posX;
	}

	/**get y position of this window*/
	int getPosY() const
	{
		return m_posY;
	}
	
	/**get width of this window*/
	int getWidth() const
	{
		return m_width;
	}

	/**get height of this window*/
	int getHeight() const
	{
		return m_height;
	}

	/**get window handle of this window*/
	HWND getHWND()
	{
		return m_hwnd;
	}

	/**show window on screen*/
	void showWindow()
	{
		ShowWindow(m_hwnd, SW_SHOW);
	}

	/**handle message queue, usually be invoked every fram*/
	bool HandleMessage();

private:
	bool initGraphicWindow(HINSTANCE hInstance, LPCTSTR title);
	void destroyGraphicWindow();

private:
	int m_posX;
	int m_posY;
	int m_width;
	int m_height;
	HWND m_hwnd;
};


