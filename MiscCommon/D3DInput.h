#pragma once

#include <Windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define MWR Microsoft::WRL
#define ADD_KEY(KeyName) KEY_##KeyName = DIK_##KeyName

namespace DeviceInput
{
    //custom key type enum mapped to microsoft keyborad device enumeration
    enum class KEY_TYPE
    {
        ADD_KEY(0),
        ADD_KEY(1),
        ADD_KEY(2),
        ADD_KEY(3),
        ADD_KEY(4),
        ADD_KEY(5),
        ADD_KEY(6),
        ADD_KEY(7),
        ADD_KEY(8),
        ADD_KEY(9),
        ADD_KEY(A),
        ADD_KEY(ABNT_C1),
        ADD_KEY(ABNT_C2),
        ADD_KEY(ADD),
        ADD_KEY(APOSTROPHE),
        ADD_KEY(APPS),
        ADD_KEY(AT),
        ADD_KEY(AX),
        ADD_KEY(B),
        ADD_KEY(BACK),
        ADD_KEY(BACKSLASH),
        ADD_KEY(C),
        ADD_KEY(CALCULATOR),
        ADD_KEY(CAPITAL),
        ADD_KEY(COLON),
        ADD_KEY(COMMA),
        ADD_KEY(CONVERT),
        ADD_KEY(D),
        ADD_KEY(DECIMAL),
        ADD_KEY(DELETE),
        ADD_KEY(DIVIDE),
        ADD_KEY(DOWN),
        ADD_KEY(E),
        ADD_KEY(END),
        ADD_KEY(EQUALS),
        ADD_KEY(ESCAPE),
        ADD_KEY(F),
        ADD_KEY(F1),
        ADD_KEY(F2),
        ADD_KEY(F3),
        ADD_KEY(F4),
        ADD_KEY(F5),
        ADD_KEY(F6),
        ADD_KEY(F7),
        ADD_KEY(F8),
        ADD_KEY(F9),
        ADD_KEY(F10),
        ADD_KEY(F11),
        ADD_KEY(F12),
        ADD_KEY(F13),
        ADD_KEY(F14),
        ADD_KEY(F15),
        ADD_KEY(G),
        ADD_KEY(GRAVE),
        ADD_KEY(H),
        ADD_KEY(HOME),
        ADD_KEY(I),
        ADD_KEY(INSERT),
        ADD_KEY(J),
        ADD_KEY(K),
        ADD_KEY(KANA),
        ADD_KEY(KANJI),
        ADD_KEY(L),
        ADD_KEY(LBRACKET),
        ADD_KEY(LCONTROL),
        ADD_KEY(LEFT),
        ADD_KEY(LMENU),
        ADD_KEY(LSHIFT),
        ADD_KEY(LWIN),
        ADD_KEY(M),
        ADD_KEY(MAIL),
        ADD_KEY(MEDIASELECT),
        ADD_KEY(MEDIASTOP),
        ADD_KEY(MINUS),
        ADD_KEY(MULTIPLY),
        ADD_KEY(MUTE),
        ADD_KEY(MYCOMPUTER),
        ADD_KEY(N),
        ADD_KEY(NEXT),
        ADD_KEY(NEXTTRACK),
        ADD_KEY(NOCONVERT),
        ADD_KEY(NUMLOCK),
        ADD_KEY(NUMPAD0),
        ADD_KEY(NUMPAD1),
        ADD_KEY(NUMPAD2),
        ADD_KEY(NUMPAD3),
        ADD_KEY(NUMPAD4),
        ADD_KEY(NUMPAD5),
        ADD_KEY(NUMPAD6),
        ADD_KEY(NUMPAD7),
        ADD_KEY(NUMPAD8),
        ADD_KEY(NUMPAD9),
        ADD_KEY(NUMPADCOMMA),
        ADD_KEY(NUMPADENTER),
        ADD_KEY(NUMPADEQUALS),
        ADD_KEY(O),
        ADD_KEY(OEM_102),
        ADD_KEY(P),
        ADD_KEY(PAUSE),
        ADD_KEY(PERIOD),
        ADD_KEY(PLAYPAUSE),
        ADD_KEY(POWER),
        ADD_KEY(PREVTRACK),
        ADD_KEY(PRIOR),
        ADD_KEY(Q),
        ADD_KEY(R),
        ADD_KEY(RBRACKET),
        ADD_KEY(RCONTROL),
        ADD_KEY(RETURN),
        ADD_KEY(RIGHT),
        ADD_KEY(RMENU),
        ADD_KEY(RSHIFT),
        ADD_KEY(RWIN),
        ADD_KEY(S),
        ADD_KEY(SCROLL),
        ADD_KEY(SEMICOLON),
        ADD_KEY(SLASH),
        ADD_KEY(SLEEP),
        ADD_KEY(SPACE),
        ADD_KEY(STOP),
        ADD_KEY(SUBTRACT),
        ADD_KEY(SYSRQ),
        ADD_KEY(T),
        ADD_KEY(TAB),
        ADD_KEY(U),
        ADD_KEY(UNDERLINE),
        ADD_KEY(UNLABELED),
        ADD_KEY(UP),
        ADD_KEY(V),
        ADD_KEY(VOLUMEDOWN),
        ADD_KEY(VOLUMEUP),
        ADD_KEY(W),
        ADD_KEY(WAKE),
        ADD_KEY(WEBBACK),
        ADD_KEY(WEBFAVORITES),
        ADD_KEY(WEBFORWARD),
        ADD_KEY(WEBHOME),
        ADD_KEY(WEBREFRESH),
        ADD_KEY(WEBSEARCH),
        ADD_KEY(WEBSTOP),
        ADD_KEY(X),
        ADD_KEY(Y),
        ADD_KEY(YEN),
        ADD_KEY(Z)
    };

    //fixed, mouse button enumeration
    enum class MOUSE_BUTTON_TYPE
    {
        BUTTON_LEFT = DIMOFS_BUTTON0,
        BUTTON_RIGHT = DIMOFS_BUTTON1,
        BUTTON_SCROLL = DIMOFS_BUTTON2
    };

    //fixel, see if the button is pressed
    enum class PRESS_STATE
    {
        PRESS_UP = 0,
        PRESS_DOWN = 1
    };

    //store mouse device axis information
    struct MouseXYZ
    {
        float x;
        float y;
        float z;
    };

    class D3DInput
    {
    public:
        D3DInput(HINSTANCE hInstance, HWND hwnd);
        ~D3DInput();
        D3DInput(const D3DInput&) = delete;
        D3DInput& operator=(const D3DInput&) = delete;

    public:
        void detectInput(float deltaTime);

        /*retrieve device data functions*/
        PRESS_STATE getKeyboardState(KEY_TYPE key);
        PRESS_STATE getMouseButtonState(MOUSE_BUTTON_TYPE mouseButton);
        MouseXYZ getMouseXYZSpeed();
        MouseXYZ getMouseXYZPosition();

    private:
        bool initKeyDevice(HWND hwnd);
        bool initMouseDevice(HWND hwnd);
        bool initD3DInput(HINSTANCE hInstance, HWND hwnd);
        void shutdownD3DInput();

    private:
        MWR::ComPtr<IDirectInput8> m_directInput;
        MWR::ComPtr<IDirectInputDevice8> m_keyDevice;
        MWR::ComPtr<IDirectInputDevice8> m_mouseDevice;

        unsigned char m_keyState[256];
        DIMOUSESTATE m_mouseState;

        bool m_isDetectingInput;
        float m_deltaTime;


    };

}