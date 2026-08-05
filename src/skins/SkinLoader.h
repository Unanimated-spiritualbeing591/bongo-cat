#pragma once
#include "../include/BongoTypes.h"
#include "../animation/SpriteSheet.h"
#include <string>
#include <memory>

namespace bongo {

// All sprite sheets for one skin pack
struct SkinData {
    std::wstring        name;
    std::wstring        path;      // root directory

    // One sheet per paw state — each may be single-frame or multi-frame
    SpriteSheet*  sheetIdle      = nullptr;
    SpriteSheet*  sheetLeftDown  = nullptr;
    SpriteSheet*  sheetRightDown = nullptr;
    SpriteSheet*  sheetBothDown  = nullptr;

    // Owned storage
    std::unique_ptr<SpriteSheet> _idle;
    std::unique_ptr<SpriteSheet> _leftDown;
    std::unique_ptr<SpriteSheet> _rightDown;
    std::unique_ptr<SpriteSheet> _bothDown;
};

// Loads a skin from a directory following the expected layout:
//   <skin>/idle.png          (1 frame)
//   <skin>/left_down.png     (1+ frames, horizontal strip)
//   <skin>/right_down.png    (1+ frames)
//   <skin>/both_down.png     (1+ frames)
// Counts frames from a companion <name>.frames.txt or defaults to 1.
class SkinLoader {
public:
    SkinLoader() = default;

    // Load skin from directory; returns nullptr on failure
    std::unique_ptr<SkinData> load(const std::wstring& dirPath);

private:
    int  readFrameCount(const std::wstring& pngPath) const;
    bool loadSheet(SpriteSheet& sheet,
                   const std::wstring& dirPath,
                   const std::wstring& filename) const;
};

} // namespace bongo
