#pragma once
#include "../include/BongoTypes.h"
#include <vector>
#include <cstdint>

namespace bongo {

struct Frame {
    std::vector<uint32_t> pixels; // ARGB
    int width  = 0;
    int height = 0;
};

// Loads a sprite sheet PNG and slices it into individual frames.
// Frames are laid out horizontally: each cell is (sheetWidth / frameCount) wide.
class SpriteSheet {
public:
    SpriteSheet() = default;
    ~SpriteSheet() = default;

    // Load PNG from disk, slice into frameCount equal columns
    bool load(const std::wstring& path, int frameCount);

    // Load from raw ARGB memory (e.g. already decoded PNG)
    bool loadFromMemory(const uint32_t* argb, int width, int height, int frameCount);

    void  unload();
    bool  isLoaded()   const { return !m_frames.empty(); }
    int   frameCount() const { return static_cast<int>(m_frames.size()); }
    int   frameWidth() const { return m_frameW; }
    int   frameHeight()const { return m_frameH; }

    const Frame* frame(int index) const;

private:
    std::vector<Frame> m_frames;
    int                m_frameW = 0;
    int                m_frameH = 0;
};

} // namespace bongo
