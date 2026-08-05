#pragma once
#include "../include/BongoTypes.h"
#include "SpriteSheet.h"

namespace bongo {

struct SkinData; // forward declared in SkinLoader.h

// Controls which animation frame to display based on PawState.
// Each PawState maps to one SpriteSheet; playback is a simple timer-driven loop.
class AnimationController {
public:
    AnimationController();
    ~AnimationController() = default;

    // Provide references to sprite sheets loaded by the skin
    void init(SkinData* skin);

    // Called once per render tick; advances frame timers
    void update();

    // Called by BongoCat when input state changes
    void setPawState(PawState state);
    PawState pawState() const { return m_state; }

    // Returns the frame that should be rendered right now
    const Frame* currentFrame() const;

    // Controls playback speed (default 60 fps)
    void setFps(int fps);

private:
    SkinData*   m_skin        = nullptr;
    PawState    m_state       = PawState::Idle;
    PawState    m_prevState   = PawState::Idle;
    int         m_frameIndex  = 0;
    float       m_accumMs     = 0.f;
    float       m_frameDurMs  = 1000.f / 30.f; // 30 fps animation by default
    DWORD       m_lastTick    = 0;

    // Returns the sprite sheet for the current state
    SpriteSheet* sheetForState(PawState s) const;
};

} // namespace bongo
