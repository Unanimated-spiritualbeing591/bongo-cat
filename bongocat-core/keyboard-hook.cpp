#include <windows.h>
#include <iostream>

class KeyboardHook {
private:
    HHOOK hook;
    static KeyboardHook* instance;
    
public:
    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* kbd = (KBDLLHOOKSTRUCT*)lParam;
            instance->onKeyPress(kbd->vkCode);
        }
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }
    
    void install() {
        instance = this;
        hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
    }
    
    void uninstall() {
        UnhookWindowsHookEx(hook);
    }
    
    virtual void onKeyPress(DWORD vkCode) = 0;
};

KeyboardHook* KeyboardHook::instance = nullptr;
