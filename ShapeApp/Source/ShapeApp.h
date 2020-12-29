#pragma once

#include "D3DApp.h"


class ShapeApp : public D3DApp
{
public:
	ShapeApp() {};
	virtual ~ShapeApp() {}

public:
	virtual bool InitializeApp(HINSTANCE hInstance);


private:
	virtual void Update(float deltaTime);
	virtual void Draw();
	virtual void OnResize(int newWidth, int newHeight) {}
	virtual std::wstring WindowTitile() { return L"Shape App"; }

	virtual void OnKeyboardAxisEvent(KEY_TYPE key) {}
	virtual void OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState) {}
	virtual void OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed) {};


};

