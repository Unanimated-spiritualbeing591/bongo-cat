#pragma once
#include "../include/BongoTypes.h"
#include <windows.h>

namespace bongo {

class OverlayWindow {
public:
    struct CreateParams {
        int   x = 100, y = 100;
        int   w = 200, h = 200;
        bool  alwaysOnTop = true;
        BYTE  opacity     = 255; // 0-255
        bool  clickThrough = true;
    };

    OverlayWindow();
    ~OverlayWindow();

    bool  create(const CreateParams& p);
    void  destroy();
    bool  isCreated() const { return m_hwnd != nullptr; }

    HWND  hwnd() const { return m_hwnd; }

    // Draw ARGB pixel buffer via UpdateLayeredWindow
    void  drawFrame(const uint32_t* argb, int w, int h);

    // Chroma key color for OBS source integration
    void  setChromaKey(Color c);
    void  clearChromaKey();

    // Position / size
    void  move(int x, int y);
    void  resize(int w, int h);

    // Opacity 0.0 - 1.0
    void  setOpacity(float opacity);

private:
    static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    bool registerClass();

    HWND    m_hwnd    = nullptr;
    HDC     m_hdc     = nullptr;
    HBITMAP m_hbm     = nullptr;
    uint32_t* m_bits  = nullptr;
    int     m_bmpW    = 0;
    int     m_bmpH    = 0;
    bool    m_chromaEnabled = false;
    Color   m_chromaColor{};
    BYTE    m_opacity = 255;
};

} // namespace bongo
