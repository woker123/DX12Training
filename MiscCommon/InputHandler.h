#pragma once

#include "D3DInput.h"
#include <memory>
#include <vector>
#include <cmath>

/**
* Keyboard event function: void OnKeyboardActionEvent(KEY_TYPE key, PRESS_STATE pState); void OnKeyboardAxisEvent(KEY_TYPE key);
* mouse button event functions: void OnMouseButtonActionEvent(MOUSE_BUTTON_TYPE mouseButton, PRESS_STATE pState), void OnMouseButtonAxisEvent(MOUSE_BUTTON_TYPE mouseButton);
* mouse move event functions: void OnMouseMove(float xPos, float yPos, float zPos, float xSpeed, float ySpeed, float zSpeed);
*/

template<class T>
class InputHanler
{
public:
	InputHanler(HINSTANCE hInstance, HWND hwnd, T* t)
		:mBindObj(t), mD3DInput(new D3DInput(hInstance, hwnd)) 
	{
		mD3DInput->detectInput(0.001f);
		InitMouseButtonState();
	}
	~InputHanler() {}

public:
	void Update(float deltaTime);
	void RegisterKeys(int nKey, DeviceInput::KEY_TYPE* keys);

private:
	//keyboard
	void SetKeyCurrentState();
	void SetKeyBeforeState();
	void DispatchKeyActionEvent();
	void DispatchKeyAxisEvent();

	//mouse
	void InitMouseButtonState();
	void SetMouseButtonBeforeState();
	void SetMouseButtonCurrentState();
	void DispatchMouseButtonActionEvent();
	void DispatchMouseButtonAxisEvent();
	void DispatchMouseMoveEvent();

private:
	T* mBindObj;
	std::shared_ptr<DeviceInput::D3DInput> mD3DInput;

private:
	//keyboard
	std::vector<DeviceInput::KEY_TYPE> mKeyTypes;
	std::vector<DeviceInput::PRESS_STATE> mKeyBeforeState;
	std::vector<DeviceInput::PRESS_STATE> mKeyCurrentState;

	//mouse
	std::vector<DeviceInput::MOUSE_BUTTON_TYPE> mMouseButtonTypes;
	std::vector<DeviceInput::PRESS_STATE> mButtonBeforeState;
	std::vector<DeviceInput::PRESS_STATE> mButtonCurrentState;
};

template<class T>
inline void InputHanler<T>::Update(float deltaTime)
{
	mD3DInput->detectInput(deltaTime);
	DispatchKeyActionEvent();
	DispatchKeyAxisEvent();
	DispatchMouseButtonActionEvent();
	DispatchMouseButtonAxisEvent();
	DispatchMouseMoveEvent();
}

template<class T>
inline void InputHanler<T>::RegisterKeys(int nKey, DeviceInput::KEY_TYPE* keys)
{
	mKeyTypes.assign(keys, keys + nKey);
	mKeyBeforeState.resize(nKey);
	mKeyCurrentState.resize(nKey);
	SetKeyBeforeState();
}

template<class T>
inline void InputHanler<T>::SetKeyCurrentState()
{
	for (int i = 0; i < (int)mKeyTypes.size(); ++i)
	{
		mKeyCurrentState[i] = mD3DInput->getKeyboardState(mKeyTypes[i]);
	}
}

template<class T>
inline void InputHanler<T>::SetKeyBeforeState()
{
	for (int i = 0; i < (int)mKeyTypes.size(); ++i)
	{
		mKeyBeforeState[i] = mD3DInput->getKeyboardState(mKeyTypes[i]);
	}
}

template<class T>
inline void InputHanler<T>::DispatchKeyActionEvent()
{
	SetKeyCurrentState();
	for (int i = 0; i < (int)mKeyTypes.size(); ++i)
	{
		if(mKeyCurrentState[i] != mKeyBeforeState[i])
			mBindObj->OnKeyboardActionEvent(mKeyTypes[i], mKeyCurrentState[i]);
	}
	SetKeyBeforeState();
}

template<class T>
inline void InputHanler<T>::DispatchKeyAxisEvent()
{
	SetKeyCurrentState();
	SetKeyBeforeState();
	for (int i = 0; i < (int)mKeyTypes.size(); ++i)
	{
		if (mKeyCurrentState[i] == DeviceInput::PRESS_STATE::PRESS_DOWN)
			mBindObj->OnKeyboardAxisEvent(mKeyTypes[i]);
	}
}

template<class T>
inline void InputHanler<T>::InitMouseButtonState()
{
	using namespace DeviceInput;
	mMouseButtonTypes.assign({ MOUSE_BUTTON_TYPE::BUTTON_LEFT, MOUSE_BUTTON_TYPE::BUTTON_RIGHT, MOUSE_BUTTON_TYPE::BUTTON_SCROLL });
	mButtonBeforeState.resize(mMouseButtonTypes.size());
	mButtonCurrentState.resize(mMouseButtonTypes.size());
	SetMouseButtonBeforeState();
}

template<class T>
inline void InputHanler<T>::SetMouseButtonBeforeState()
{
	for (int i = 0; i < (int)mMouseButtonTypes.size(); ++i)
	{
		mButtonBeforeState[i] = mD3DInput->getMouseButtonState(mMouseButtonTypes[i]);
	}
}

template<class T>
inline void InputHanler<T>::SetMouseButtonCurrentState()
{
	for (int i = 0; i < (int)mMouseButtonTypes.size(); ++i)
	{
		mButtonCurrentState[i] = mD3DInput->getMouseButtonState(mMouseButtonTypes[i]);
	}
}

template<class T>
inline void InputHanler<T>::DispatchMouseButtonActionEvent()
{
	SetMouseButtonCurrentState();
	for (int i = 0; i < (int)mMouseButtonTypes.size(); ++i)
	{
		if (mButtonCurrentState[i] != mButtonBeforeState[i])
			mBindObj->OnMouseButtonActionEvent(mMouseButtonTypes[i] ,mButtonCurrentState[i]);
	}
}

template<class T>
inline void InputHanler<T>::DispatchMouseButtonAxisEvent()
{
	SetMouseButtonCurrentState();
	SetMouseButtonBeforeState();
	for (int i = 0; i < (int)mMouseButtonTypes.size(); ++i)
	{
		if (mButtonCurrentState[i] == DeviceInput::PRESS_STATE::PRESS_DOWN)
			mBindObj->OnMouseButtonAxisEvent(mMouseButtonTypes[i]);
	}
}

template<class T>
inline void InputHanler<T>::DispatchMouseMoveEvent()
{
	DeviceInput::MouseXYZ mouseSpeed = mD3DInput->getMouseXYZSpeed();

	if (std::abs(mouseSpeed.x) > 0.0 || std::abs(mouseSpeed.y) > 0.0 || std::abs(mouseSpeed.z) > 0.0)
	{
		DeviceInput::MouseXYZ mousePos = mD3DInput->getMouseXYZPosition();
		mBindObj->OnMouseMove(mousePos.x, mousePos.y, mousePos.z, mouseSpeed.x, mouseSpeed.y, mouseSpeed.z);
	}
}
