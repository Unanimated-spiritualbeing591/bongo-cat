#pragma once
#include "../include/BongoTypes.h"
#include <windows.h>
#include <functional>
#include <vector>

namespace bongo {

using InputCallback = std::function<void(const InputEvent&)>;

class InputTracker {
public:
    InputTracker();
    ~InputTracker();

    bool        install();
    void        uninstall();
    bool        isInstalled() const { return m_installed; }

    void        addCallback(InputCallback cb);
    void        clearCallbacks();

    // Check current state without waiting for event
    bool        isKeyDown(DWORD vkCode) const;
    bool        isMouseButtonDown(MouseButton btn) const;
    POINT       getMousePos() const;

private:
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK mouseProc   (int nCode, WPARAM wParam, LPARAM lParam);

    void dispatch(const InputEvent& ev);

    static InputTracker*        s_instance;
    HHOOK                       m_kbHook    = nullptr;
    HHOOK                       m_mouseHook = nullptr;
    bool                        m_installed = false;
    std::vector<InputCallback>  m_callbacks;

    // Bitmask of currently-held VK codes (0x00..0xFF)
    uint8_t                     m_keyState[256] = {};
    bool                        m_mouseState[3] = {}; // L/R/M
    POINT                       m_mousePos      = {};
};

} // namespace bongo
