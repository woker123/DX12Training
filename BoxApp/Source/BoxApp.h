#pragma once

#include "D3DApp.h"


class BoxApp : public D3DApp
{
public:
	BoxApp() {}
	virtual ~BoxApp() {}

public:
	virtual bool InitializeApp(HINSTANCE hInstance) override;


private:
	//input 
	virtual void OnKeyboardActionEvent(KEY_TYPE key, PRESS_STATE pState) override;
	virtual void OnKeyboardAxisEvent(KEY_TYPE key) override;
	virtual void OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState) override;
	virtual void OnMouseButtonAxisEvent(MOUSE_BUTTON_TYPE mouseButton) override;
	virtual void OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed) override;

private:


};
