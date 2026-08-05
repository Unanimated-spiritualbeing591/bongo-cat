#include "SkinManager.h"
#include <windows.h>
#include <filesystem>

namespace bongo {

namespace fs = std::filesystem;

SkinManager::SkinManager() = default;

bool SkinManager::loadSkin(const std::wstring& dirPath) {
    auto data = m_loader.load(dirPath);
    if (!data) return false;

    m_skins.push_back(std::move(data));
    m_activeIndex = static_cast<int>(m_skins.size()) - 1;
    m_active      = m_skins.back().get();
    return true;
}

void SkinManager::scanDirectory(const std::wstring& skinsRoot) {
    if (!fs::exists(skinsRoot)) return;

    for (auto& entry : fs::directory_iterator(skinsRoot)) {
        if (!entry.is_directory()) continue;
        auto data = m_loader.load(entry.path().wstring());
        if (data) m_skins.push_back(std::move(data));
    }

    if (!m_skins.empty() && m_activeIndex < 0) {
        m_activeIndex = 0;
        m_active      = m_skins[0].get();
    }
}

bool SkinManager::setActiveSkin(int index) {
    if (index < 0 || index >= (int)m_skins.size()) return false;
    m_activeIndex = index;
    m_active      = m_skins[index].get();
    return true;
}

bool SkinManager::setActiveSkinByName(const std::wstring& name) {
    for (int i = 0; i < (int)m_skins.size(); ++i) {
        if (m_skins[i]->name == name) return setActiveSkin(i);
    }
    return false;
}

SkinData* SkinManager::skin(int index) const {
    if (index < 0 || index >= (int)m_skins.size()) return nullptr;
    return m_skins[index].get();
}

std::vector<std::wstring> SkinManager::skinNames() const {
    std::vector<std::wstring> names;
    names.reserve(m_skins.size());
    for (auto& s : m_skins) names.push_back(s->name);
    return names;
}

} // namespace bongo
