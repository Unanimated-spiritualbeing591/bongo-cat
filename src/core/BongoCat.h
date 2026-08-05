#pragma once
#include "../include/BongoTypes.h"
#include "InputTracker.h"
#include "../animation/AnimationController.h"
#include "../overlay/OverlayWindow.h"
#include "../skins/SkinManager.h"
#include <memory>
#include <atomic>

namespace bongo {

struct AppConfig {
    std::wstring skinPath;       // path to active skin directory
    int          windowX  = 100;
    int          windowY  = 100;
    int          windowW  = 200;
    int          windowH  = 200;
    bool         alwaysOnTop = true;
    bool         chromaKey   = false;
    Color        chromaColor = Color::fromRGB(0, 255, 0);
    float        opacity     = 1.0f;
    DWORD        leftKey     = 'Z';
    DWORD        rightKey    = 'X';
};

class BongoCat {
public:
    BongoCat();
    ~BongoCat();

    // Load config, create window, install hooks
    bool  init(const AppConfig& cfg);

    // Main message loop — blocks until quit
    int   run();

    // Gracefully post WM_QUIT
    void  quit();

    // Accessors
    SkinManager*          skinManager()          { return m_skins.get(); }
    OverlayWindow*        overlayWindow()         { return m_overlay.get(); }
    AnimationController*  animationController()   { return m_anim.get(); }
    InputTracker*         inputTracker()          { return m_input.get(); }

private:
    void onInput(const InputEvent& ev);
    void onTick();

    AppConfig                            m_cfg;
    std::unique_ptr<InputTracker>        m_input;
    std::unique_ptr<AnimationController> m_anim;
    std::unique_ptr<OverlayWindow>       m_overlay;
    std::unique_ptr<SkinManager>         m_skins;
    std::atomic<bool>                    m_running{false};
};

} // namespace bongo
