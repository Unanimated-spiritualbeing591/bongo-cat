#include "SpriteSheet.h"
#include <windows.h>
#include <gdiplus.h>
#include <stdexcept>

#pragma comment(lib, "gdiplus.lib")

namespace bongo {

bool SpriteSheet::load(const std::wstring& path, int frameCount) {
    // Use GDI+ to decode the PNG
    Gdiplus::GdiplusStartupInput gsi;
    ULONG_PTR token;
    Gdiplus::GdiplusStartup(&token, &gsi, nullptr);

    Gdiplus::Bitmap bmp(path.c_str());
    if (bmp.GetLastStatus() != Gdiplus::Ok) {
        Gdiplus::GdiplusShutdown(token);
        return false;
    }

    UINT w = bmp.GetWidth();
    UINT h = bmp.GetHeight();
    std::vector<uint32_t> raw(w * h);

    Gdiplus::BitmapData data;
    Gdiplus::Rect rect(0, 0, (INT)w, (INT)h);
    bmp.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data);
    memcpy(raw.data(), data.Scan0, w * h * 4);
    bmp.UnlockBits(&data);

    Gdiplus::GdiplusShutdown(token);

    return loadFromMemory(raw.data(), (int)w, (int)h, frameCount);
}

bool SpriteSheet::loadFromMemory(const uint32_t* argb, int width, int height, int frameCount) {
    if (frameCount <= 0 || width <= 0 || height <= 0) return false;
    if (width % frameCount != 0) return false; // sheet must divide evenly

    m_frameW = width / frameCount;
    m_frameH = height;
    m_frames.resize(frameCount);

    for (int i = 0; i < frameCount; ++i) {
        Frame& f = m_frames[i];
        f.width  = m_frameW;
        f.height = m_frameH;
        f.pixels.resize(m_frameW * m_frameH);

        for (int row = 0; row < m_frameH; ++row) {
            const uint32_t* src = argb + row * width + i * m_frameW;
            uint32_t*       dst = f.pixels.data() + row * m_frameW;
            memcpy(dst, src, m_frameW * sizeof(uint32_t));
        }
    }
    return true;
}

void SpriteSheet::unload() {
    m_frames.clear();
    m_frameW = m_frameH = 0;
}

const Frame* SpriteSheet::frame(int index) const {
    if (index < 0 || index >= (int)m_frames.size()) return nullptr;
    return &m_frames[index];
}

} // namespace bongo
