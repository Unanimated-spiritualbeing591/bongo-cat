#pragma once
#include "SkinLoader.h"
#include <vector>
#include <string>

namespace bongo {

class SkinManager {
public:
    SkinManager();
    ~SkinManager() = default;

    // Load a skin by directory path; makes it active immediately
    bool        loadSkin(const std::wstring& dirPath);

    // Discover all skins inside a skins root folder
    void        scanDirectory(const std::wstring& skinsRoot);

    // Switch active skin by index (from scanned list)
    bool        setActiveSkin(int index);

    // Switch active skin by name
    bool        setActiveSkinByName(const std::wstring& name);

    SkinData*   activeSkin() const { return m_active; }
    int         activeSkinIndex() const { return m_activeIndex; }

    int         skinCount() const { return static_cast<int>(m_skins.size()); }
    SkinData*   skin(int index) const;

    // Returns list of skin names in scan order
    std::vector<std::wstring> skinNames() const;

private:
    SkinLoader                               m_loader;
    std::vector<std::unique_ptr<SkinData>>   m_skins;
    SkinData*                                m_active      = nullptr;
    int                                      m_activeIndex = -1;
};

} // namespace bongo
