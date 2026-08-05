#include "ChromaKey.h"
#include <cstdlib>

namespace bongo {

void ChromaKey::setColor(Color c, int tolerance) {
    m_color     = c;
    m_tolerance = tolerance;
    m_enabled   = true;
}

void ChromaKey::disable() {
    m_enabled = false;
}

// Returns true if the pixel's RGB channels are within tolerance of the key color.
inline bool ChromaKey::matches(uint32_t pixel) const {
    // Layout: 0xAARRGGBB (GDI+ ARGB)
    int r = (pixel >> 16) & 0xFF;
    int g = (pixel >>  8) & 0xFF;
    int b = (pixel      ) & 0xFF;

    return std::abs(r - (int)m_color.r) <= m_tolerance
        && std::abs(g - (int)m_color.g) <= m_tolerance
        && std::abs(b - (int)m_color.b) <= m_tolerance;
}

void ChromaKey::apply(uint32_t* argb, int width, int height) const {
    if (!m_enabled) return;
    int total = width * height;
    for (int i = 0; i < total; ++i) {
        if (matches(argb[i])) {
            argb[i] = 0x00000000; // fully transparent
        }
    }
}

void ChromaKey::applyTo(const uint32_t* src, uint32_t* dst, int width, int height) const {
    int total = width * height;
    if (!m_enabled) {
        memcpy(dst, src, total * sizeof(uint32_t));
        return;
    }
    for (int i = 0; i < total; ++i) {
        dst[i] = matches(src[i]) ? 0x00000000 : src[i];
    }
}

} // namespace bongo
