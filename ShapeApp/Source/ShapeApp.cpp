#include "ShapeApp.h"

bool ShapeApp::InitializeApp(HINSTANCE hInstance)
{
	if (!D3DApp::InitializeApp(hInstance))
		return false;


	


	return true;
}

void ShapeApp::Update(float deltaTime)
{
	D3DApp::Update(deltaTime);

}

void ShapeApp::Draw()
{
	D3DApp::Draw();



}

