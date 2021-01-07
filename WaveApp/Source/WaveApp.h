#pragma once


#include "D3DApp.h"


class WaveApp : public D3DApp
{
public:
	WaveApp() {}
	virtual ~WaveApp() {}

public:
	virtual bool InitializeApp(HINSTANCE hInstance);

private:
	virtual void OnKeyboardAxisEvent(KEY_TYPE key);
	virtual void OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState);
	virtual void OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed);

	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual std::wstring WindowTitile() { return L"Wave App"; }

};

