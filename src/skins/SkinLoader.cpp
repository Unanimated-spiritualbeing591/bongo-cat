#include "SkinLoader.h"
#include <windows.h>
#include <fstream>
#include <filesystem>

namespace bongo {

namespace fs = std::filesystem;

bool SkinLoader::loadSheet(SpriteSheet& sheet,
                            const std::wstring& dirPath,
                            const std::wstring& filename) const {
    std::wstring fullPath = dirPath + L"\\" + filename;
    if (!fs::exists(fullPath)) return false;

    int frames = readFrameCount(fullPath);
    return sheet.load(fullPath, frames);
}

int SkinLoader::readFrameCount(const std::wstring& pngPath) const {
    // Look for a sidecar file: e.g. idle.png → idle.frames.txt
    std::wstring sidecar = pngPath.substr(0, pngPath.rfind(L'.')) + L".frames.txt";
    if (fs::exists(sidecar)) {
        std::wifstream f(sidecar);
        int n = 1;
        f >> n;
        if (n > 0) return n;
    }
    return 1;
}

std::unique_ptr<SkinData> SkinLoader::load(const std::wstring& dirPath) {
    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) return nullptr;

    auto skin = std::make_unique<SkinData>();
    skin->path = dirPath;
    skin->name = fs::path(dirPath).filename().wstring();

    // Allocate sprite sheets
    skin->_idle      = std::make_unique<SpriteSheet>();
    skin->_leftDown  = std::make_unique<SpriteSheet>();
    skin->_rightDown = std::make_unique<SpriteSheet>();
    skin->_bothDown  = std::make_unique<SpriteSheet>();

    // idle.png is mandatory
    if (!loadSheet(*skin->_idle, dirPath, L"idle.png")) return nullptr;

    // Others fall back to idle if missing
    if (!loadSheet(*skin->_leftDown,  dirPath, L"left_down.png"))  *skin->_leftDown  = *skin->_idle;
    if (!loadSheet(*skin->_rightDown, dirPath, L"right_down.png")) *skin->_rightDown = *skin->_idle;
    if (!loadSheet(*skin->_bothDown,  dirPath, L"both_down.png"))  *skin->_bothDown  = *skin->_idle;

    skin->sheetIdle      = skin->_idle.get();
    skin->sheetLeftDown  = skin->_leftDown.get();
    skin->sheetRightDown = skin->_rightDown.get();
    skin->sheetBothDown  = skin->_bothDown.get();

    return skin;
}

} // namespace bongo
