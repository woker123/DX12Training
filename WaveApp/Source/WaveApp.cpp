#include "WaveApp.h"

bool WaveApp::InitializeApp(HINSTANCE hInstance)
{
	if (!D3DApp::InitializeApp(hInstance))
		return false;

	return true;
}

void WaveApp::OnKeyboardAxisEvent(KEY_TYPE key)
{
}

void WaveApp::OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState)
{
}

void WaveApp::OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed)
{
}

void WaveApp::Update(float deltaTime)
{
	D3DApp::Update(deltaTime);


}

void WaveApp::Draw()
{
	D3DApp::Draw();


}
