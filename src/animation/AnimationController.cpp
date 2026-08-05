#include "AnimationController.h"
#include "../skins/SkinLoader.h"
#include <windows.h>

namespace bongo {

AnimationController::AnimationController() {
    m_lastTick = GetTickCount();
}

void AnimationController::init(SkinData* skin) {
    m_skin       = skin;
    m_frameIndex = 0;
    m_accumMs    = 0.f;
    m_lastTick   = GetTickCount();
}

void AnimationController::setFps(int fps) {
    if (fps > 0) m_frameDurMs = 1000.f / fps;
}

void AnimationController::setPawState(PawState state) {
    if (state != m_state) {
        m_prevState  = m_state;
        m_state      = state;
        m_frameIndex = 0;  // restart animation for new state
        m_accumMs    = 0.f;
    }
}

void AnimationController::update() {
    DWORD now = GetTickCount();
    float dt  = static_cast<float>(now - m_lastTick);
    m_lastTick = now;

    SpriteSheet* sheet = sheetForState(m_state);
    if (!sheet || sheet->frameCount() <= 1) return;

    m_accumMs += dt;
    while (m_accumMs >= m_frameDurMs) {
        m_accumMs  -= m_frameDurMs;
        m_frameIndex = (m_frameIndex + 1) % sheet->frameCount();
    }
}

const Frame* AnimationController::currentFrame() const {
    SpriteSheet* sheet = sheetForState(m_state);
    if (!sheet) return nullptr;
    return sheet->frame(m_frameIndex);
}

SpriteSheet* AnimationController::sheetForState(PawState s) const {
    if (!m_skin) return nullptr;
    switch (s) {
        case PawState::Idle:      return m_skin->sheetIdle;
        case PawState::LeftDown:  return m_skin->sheetLeftDown;
        case PawState::RightDown: return m_skin->sheetRightDown;
        case PawState::BothDown:  return m_skin->sheetBothDown;
    }
    return m_skin->sheetIdle;
}

} // namespace bongo
