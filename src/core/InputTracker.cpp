#include "InputTracker.h"
#include <stdexcept>

namespace bongo {

InputTracker* InputTracker::s_instance = nullptr;

InputTracker::InputTracker() {
    s_instance = this;
}

InputTracker::~InputTracker() {
    uninstall();
    s_instance = nullptr;
}

bool InputTracker::install() {
    if (m_installed) return true;

    m_kbHook = SetWindowsHookExW(WH_KEYBOARD_LL, keyboardProc, nullptr, 0);
    if (!m_kbHook) return false;

    m_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, mouseProc, nullptr, 0);
    if (!m_mouseHook) {
        UnhookWindowsHookEx(m_kbHook);
        m_kbHook = nullptr;
        return false;
    }

    m_installed = true;
    return true;
}

void InputTracker::uninstall() {
    if (m_kbHook)    { UnhookWindowsHookEx(m_kbHook);    m_kbHook    = nullptr; }
    if (m_mouseHook) { UnhookWindowsHookEx(m_mouseHook); m_mouseHook = nullptr; }
    m_installed = false;
}

void InputTracker::addCallback(InputCallback cb) {
    m_callbacks.push_back(std::move(cb));
}

void InputTracker::clearCallbacks() {
    m_callbacks.clear();
}

bool InputTracker::isKeyDown(DWORD vkCode) const {
    if (vkCode >= 256) return false;
    return m_keyState[vkCode] != 0;
}

bool InputTracker::isMouseButtonDown(MouseButton btn) const {
    int idx = static_cast<int>(btn) - 1;
    if (idx < 0 || idx > 2) return false;
    return m_mouseState[idx];
}

POINT InputTracker::getMousePos() const {
    return m_mousePos;
}

void InputTracker::dispatch(const InputEvent& ev) {
    for (auto& cb : m_callbacks) cb(ev);
}

// ---- Low-level keyboard hook ----
LRESULT CALLBACK InputTracker::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && s_instance) {
        auto* kbs = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        InputEvent ev{};
        ev.vkCode    = kbs->vkCode;
        ev.timestamp = kbs->time;
        ev.button    = MouseButton::None;
        ev.pos       = {};

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            ev.type = InputEvent::Type::KeyDown;
            if (kbs->vkCode < 256) s_instance->m_keyState[kbs->vkCode] = 1;
        } else {
            ev.type = InputEvent::Type::KeyUp;
            if (kbs->vkCode < 256) s_instance->m_keyState[kbs->vkCode] = 0;
        }
        s_instance->dispatch(ev);
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ---- Low-level mouse hook ----
LRESULT CALLBACK InputTracker::mouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && s_instance) {
        auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        InputEvent ev{};
        ev.pos.x     = ms->pt.x;
        ev.pos.y     = ms->pt.y;
        ev.timestamp = ms->time;
        ev.vkCode    = 0;
        s_instance->m_mousePos = ms->pt;

        switch (wParam) {
            case WM_MOUSEMOVE:
                ev.type   = InputEvent::Type::MouseMove;
                ev.button = MouseButton::None;
                break;
            case WM_LBUTTONDOWN:
                ev.type   = InputEvent::Type::MouseDown;
                ev.button = MouseButton::Left;
                s_instance->m_mouseState[0] = true;
                break;
            case WM_LBUTTONUP:
                ev.type   = InputEvent::Type::MouseUp;
                ev.button = MouseButton::Left;
                s_instance->m_mouseState[0] = false;
                break;
            case WM_RBUTTONDOWN:
                ev.type   = InputEvent::Type::MouseDown;
                ev.button = MouseButton::Right;
                s_instance->m_mouseState[1] = true;
                break;
            case WM_RBUTTONUP:
                ev.type   = InputEvent::Type::MouseUp;
                ev.button = MouseButton::Right;
                s_instance->m_mouseState[1] = false;
                break;
            case WM_MBUTTONDOWN:
                ev.type   = InputEvent::Type::MouseDown;
                ev.button = MouseButton::Middle;
                s_instance->m_mouseState[2] = true;
                break;
            case WM_MBUTTONUP:
                ev.type   = InputEvent::Type::MouseUp;
                ev.button = MouseButton::Middle;
                s_instance->m_mouseState[2] = false;
                break;
            default:
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }
        s_instance->dispatch(ev);
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

} // namespace bongo
