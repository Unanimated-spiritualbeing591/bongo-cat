#pragma once
#include <windows.h>
#include <string>
#include <memory>

namespace bongo {

enum class PawState {
    Idle = 0,
    LeftDown,
    RightDown,
    BothDown
};

enum class MouseButton {
    None = 0,
    Left,
    Right,
    Middle
};

struct InputEvent {
    enum class Type { KeyDown, KeyUp, MouseDown, MouseUp, MouseMove } type;
    DWORD       vkCode;        // virtual-key for keyboard events
    MouseButton button;        // for mouse events
    POINT       pos;           // cursor position
    DWORD       timestamp;
};

struct Vec2 {
    float x = 0.f, y = 0.f;
};

struct Rect {
    int x = 0, y = 0, w = 0, h = 0;
};

struct Color {
    uint8_t r = 0, g = 0, b = 0, a = 255;
    static Color fromRGB(uint8_t r, uint8_t g, uint8_t b) { return {r, g, b, 255}; }
    static Color transparent()                             { return {0, 0, 0, 0}; }
};

} // namespace bongo
