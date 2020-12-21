#include "BoxApp.h"

bool BoxApp::InitializeApp(HINSTANCE hInstance)
{
    if(!D3DApp::InitializeApp(hInstance))
        return false;
    return true;
}

void BoxApp::OnKeyboardActionEvent(KEY_TYPE key, PRESS_STATE pState)
{
   
}

void BoxApp::OnKeyboardAxisEvent(KEY_TYPE key)
{
    
}

void BoxApp::OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState)
{
    
}

void BoxApp::OnMouseButtonAxisEvent(MOUSE_BUTTON_TYPE mouseButton)
{
   

}

void BoxApp::OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed)
{
    
}
