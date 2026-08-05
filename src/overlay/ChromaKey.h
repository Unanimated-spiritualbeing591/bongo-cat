#pragma once
#include "../include/BongoTypes.h"
#include <vector>

namespace bongo {

// Replaces a solid chroma-key color with full transparency in an ARGB buffer.
// Used to produce clean OBS overlays without a separate key filter.
class ChromaKey {
public:
    ChromaKey() = default;

    // Set the key color and tolerance (0-255 per channel)
    void  setColor(Color c, int tolerance = 30);
    void  disable();
    bool  isEnabled() const { return m_enabled; }

    Color keyColor()  const { return m_color; }
    int   tolerance() const { return m_tolerance; }

    // Process a buffer in-place: matching pixels → alpha = 0
    void  apply(uint32_t* argb, int width, int height) const;

    // Same as apply but writes result into a separate output buffer
    void  applyTo(const uint32_t* src, uint32_t* dst, int width, int height) const;

private:
    bool  matches(uint32_t pixel) const;

    Color m_color{};
    int   m_tolerance = 30;
    bool  m_enabled   = false;
};

} // namespace bongo
