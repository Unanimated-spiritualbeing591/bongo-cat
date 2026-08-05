#include "OverlayWindow.h"
#include "ChromaKey.h"
#include <stdexcept>

namespace bongo {

static const wchar_t* kClassName = L"BongoCatOverlay";

OverlayWindow::OverlayWindow() = default;

OverlayWindow::~OverlayWindow() {
    destroy();
}

bool OverlayWindow::registerClass() {
    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = wndProc;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = kClassName;
    ATOM atom = RegisterClassExW(&wc);
    return atom != 0 || GetLastError() == ERROR_CLASS_ALREADY_EXISTS;
}

bool OverlayWindow::create(const CreateParams& p) {
    if (!registerClass()) return false;

    m_opacity = p.opacity;

    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    if (p.clickThrough) exStyle |= WS_EX_TRANSPARENT;
    if (p.alwaysOnTop)  exStyle |= WS_EX_TOPMOST;

    m_hwnd = CreateWindowExW(
        exStyle,
        kClassName,
        L"BongoCat",
        WS_POPUP,
        p.x, p.y, p.w, p.h,
        nullptr, nullptr,
        GetModuleHandleW(nullptr),
        this
    );
    if (!m_hwnd) return false;

    // Initial transparent content
    uint32_t blank = 0;
    drawFrame(&blank, 1, 1);

    ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
    UpdateWindow(m_hwnd);
    return true;
}

void OverlayWindow::destroy() {
    if (m_hbm) { DeleteObject(m_hbm); m_hbm = nullptr; m_bits = nullptr; }
    if (m_hdc) { DeleteDC(m_hdc);     m_hdc = nullptr; }
    if (m_hwnd){ DestroyWindow(m_hwnd); m_hwnd = nullptr; }
}

void OverlayWindow::drawFrame(const uint32_t* argb, int w, int h) {
    if (!m_hwnd) return;

    // Re-create DIB if size changed
    if (w != m_bmpW || h != m_bmpH || !m_hbm) {
        if (m_hbm) { DeleteObject(m_hbm); m_hbm = nullptr; m_bits = nullptr; }
        if (!m_hdc) m_hdc = CreateCompatibleDC(nullptr);

        BITMAPINFOHEADER bih{};
        bih.biSize        = sizeof(bih);
        bih.biWidth       = w;
        bih.biHeight      = -h; // top-down
        bih.biPlanes      = 1;
        bih.biBitCount    = 32;
        bih.biCompression = BI_RGB;

        BITMAPINFO bi{};
        bi.bmiHeader = bih;
        m_hbm = CreateDIBSection(m_hdc, &bi, DIB_RGB_COLORS,
                                  reinterpret_cast<void**>(&m_bits), nullptr, 0);
        if (!m_hbm) return;
        SelectObject(m_hdc, m_hbm);
        m_bmpW = w;
        m_bmpH = h;
    }

    // Copy pixels; optionally apply chroma key
    int total = w * h;
    if (m_chromaEnabled) {
        ChromaKey ck;
        ck.setColor(m_chromaColor);
        ck.applyTo(argb, m_bits, w, h);
    } else {
        memcpy(m_bits, argb, total * 4);
    }

    // UpdateLayeredWindow blends per-pixel alpha onto the desktop
    POINT ptSrc  = {0, 0};
    SIZE  sz     = {w, h};
    BLENDFUNCTION bf{ AC_SRC_OVER, 0, m_opacity, AC_SRC_ALPHA };

    POINT ptDst{};
    GetWindowRect(m_hwnd, reinterpret_cast<RECT*>(&ptDst)); // abuse RECT for x,y
    // Actually get top-left properly:
    RECT rc{};
    GetWindowRect(m_hwnd, &rc);
    ptDst = {rc.left, rc.top};

    UpdateLayeredWindow(m_hwnd, nullptr, &ptDst, &sz,
                        m_hdc, &ptSrc, 0, &bf, ULW_ALPHA);
}

void OverlayWindow::setChromaKey(Color c) {
    m_chromaEnabled = true;
    m_chromaColor   = c;
}

void OverlayWindow::clearChromaKey() {
    m_chromaEnabled = false;
}

void OverlayWindow::move(int x, int y) {
    if (m_hwnd) SetWindowPos(m_hwnd, nullptr, x, y, 0, 0,
                              SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void OverlayWindow::resize(int w, int h) {
    if (m_hwnd) SetWindowPos(m_hwnd, nullptr, 0, 0, w, h,
                              SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void OverlayWindow::setOpacity(float opacity) {
    m_opacity = static_cast<BYTE>(opacity * 255.f);
}

LRESULT CALLBACK OverlayWindow::wndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_DESTROY) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

} // namespace bongo
