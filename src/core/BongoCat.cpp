#include "BongoCat.h"
#include <stdexcept>

namespace bongo {

BongoCat::BongoCat() = default;
BongoCat::~BongoCat() { quit(); }

bool BongoCat::init(const AppConfig& cfg) {
    m_cfg = cfg;

    // Input tracker
    m_input = std::make_unique<InputTracker>();
    m_input->addCallback([this](const InputEvent& ev) { onInput(ev); });
    if (!m_input->install()) return false;

    // Skin manager
    m_skins = std::make_unique<SkinManager>();
    if (!cfg.skinPath.empty()) {
        m_skins->loadSkin(cfg.skinPath);
    }

    // Animation controller
    m_anim = std::make_unique<AnimationController>();
    m_anim->init(m_skins->activeSkin());

    // Overlay window
    m_overlay = std::make_unique<OverlayWindow>();
    OverlayWindow::CreateParams wp{};
    wp.x         = cfg.windowX;
    wp.y         = cfg.windowY;
    wp.w         = cfg.windowW;
    wp.h         = cfg.windowH;
    wp.alwaysOnTop = cfg.alwaysOnTop;
    wp.opacity   = static_cast<BYTE>(cfg.opacity * 255.f);
    if (!m_overlay->create(wp)) return false;

    if (cfg.chromaKey) {
        m_overlay->setChromaKey(cfg.chromaColor);
    }

    m_running = true;
    return true;
}

int BongoCat::run() {
    const DWORD targetMs = 16; // ~60 fps render tick
    MSG msg{};

    while (m_running) {
        DWORD start = GetTickCount();

        // Drain message queue
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                m_running = false;
                return static_cast<int>(msg.wParam);
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        onTick();

        DWORD elapsed = GetTickCount() - start;
        if (elapsed < targetMs) Sleep(targetMs - elapsed);
    }
    return 0;
}

void BongoCat::quit() {
    if (m_running.exchange(false)) {
        PostQuitMessage(0);
    }
    if (m_input) m_input->uninstall();
}

void BongoCat::onInput(const InputEvent& ev) {
    if (!m_anim) return;

    bool leftDown  = m_input->isKeyDown(m_cfg.leftKey)
                  || m_input->isMouseButtonDown(MouseButton::Left);
    bool rightDown = m_input->isKeyDown(m_cfg.rightKey)
                  || m_input->isMouseButtonDown(MouseButton::Right);

    PawState state = PawState::Idle;
    if (leftDown && rightDown) state = PawState::BothDown;
    else if (leftDown)         state = PawState::LeftDown;
    else if (rightDown)        state = PawState::RightDown;

    m_anim->setPawState(state);
}

void BongoCat::onTick() {
    if (!m_anim || !m_overlay) return;

    m_anim->update();
    const auto* frame = m_anim->currentFrame();
    if (frame) {
        m_overlay->drawFrame(frame->pixels.data(),
                             frame->width,
                             frame->height);
    }
}

} // namespace bongo
