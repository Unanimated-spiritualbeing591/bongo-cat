#include <cassert>
#include <iostream>
#include "../src/animation/AnimationController.h"
#include "../src/skins/SkinLoader.h"

using namespace bongo;

// Helper: build a minimal single-frame SkinData without loading PNGs
static std::unique_ptr<SkinData> makeMockSkin() {
    auto skin = std::make_unique<SkinData>();
    skin->name = L"mock";

    // Single 4x4 frame, all opaque white
    std::vector<uint32_t> pixels(4 * 4, 0xFFFFFFFF);

    skin->_idle      = std::make_unique<SpriteSheet>();
    skin->_leftDown  = std::make_unique<SpriteSheet>();
    skin->_rightDown = std::make_unique<SpriteSheet>();
    skin->_bothDown  = std::make_unique<SpriteSheet>();

    skin->_idle->loadFromMemory(pixels.data(), 4, 4, 1);
    skin->_leftDown->loadFromMemory(pixels.data(), 4, 4, 1);
    skin->_rightDown->loadFromMemory(pixels.data(), 4, 4, 1);
    skin->_bothDown->loadFromMemory(pixels.data(), 4, 4, 1);

    skin->sheetIdle      = skin->_idle.get();
    skin->sheetLeftDown  = skin->_leftDown.get();
    skin->sheetRightDown = skin->_rightDown.get();
    skin->sheetBothDown  = skin->_bothDown.get();

    return skin;
}

static void test_default_state() {
    auto skin = makeMockSkin();
    AnimationController ac;
    ac.init(skin.get());
    assert(ac.pawState() == PawState::Idle);
    std::cout << "[PASS] default_state\n";
}

static void test_state_transition() {
    auto skin = makeMockSkin();
    AnimationController ac;
    ac.init(skin.get());

    ac.setPawState(PawState::LeftDown);
    assert(ac.pawState() == PawState::LeftDown);

    ac.setPawState(PawState::RightDown);
    assert(ac.pawState() == PawState::RightDown);

    ac.setPawState(PawState::BothDown);
    assert(ac.pawState() == PawState::BothDown);

    ac.setPawState(PawState::Idle);
    assert(ac.pawState() == PawState::Idle);
    std::cout << "[PASS] state_transition\n";
}

static void test_current_frame_not_null() {
    auto skin = makeMockSkin();
    AnimationController ac;
    ac.init(skin.get());

    const Frame* f = ac.currentFrame();
    assert(f != nullptr);
    assert(f->width  == 4);
    assert(f->height == 4);
    assert(f->pixels.size() == 16);
    std::cout << "[PASS] current_frame_not_null\n";
}

static void test_frame_resets_on_state_change() {
    // Build a 3-frame sheet to verify frame index resets
    std::vector<uint32_t> pixels(12 * 4, 0xFFFFFFFF); // 12 wide, 4 tall → 3 frames of 4x4

    auto skin = std::make_unique<SkinData>();
    skin->_idle      = std::make_unique<SpriteSheet>();
    skin->_leftDown  = std::make_unique<SpriteSheet>();
    skin->_rightDown = std::make_unique<SpriteSheet>();
    skin->_bothDown  = std::make_unique<SpriteSheet>();

    skin->_idle->loadFromMemory     (pixels.data(), 12, 4, 3);
    skin->_leftDown->loadFromMemory (pixels.data(), 12, 4, 3);
    skin->_rightDown->loadFromMemory(pixels.data(), 12, 4, 3);
    skin->_bothDown->loadFromMemory (pixels.data(), 12, 4, 3);

    skin->sheetIdle      = skin->_idle.get();
    skin->sheetLeftDown  = skin->_leftDown.get();
    skin->sheetRightDown = skin->_rightDown.get();
    skin->sheetBothDown  = skin->_bothDown.get();

    AnimationController ac;
    ac.setFps(1000); // very fast so frames advance quickly
    ac.init(skin.get());

    // Advance a couple of ticks then change state — frame index must reset to 0
    for (int i = 0; i < 5; ++i) ac.update();
    ac.setPawState(PawState::LeftDown);
    // After state change, currentFrame must be valid (frame 0 of left_down)
    assert(ac.currentFrame() != nullptr);
    std::cout << "[PASS] frame_resets_on_state_change\n";
}

static void test_update_no_crash_without_skin() {
    AnimationController ac; // no init
    ac.update();             // must not crash
    assert(ac.currentFrame() == nullptr);
    std::cout << "[PASS] update_no_crash_without_skin\n";
}

int main() {
    test_default_state();
    test_state_transition();
    test_current_frame_not_null();
    test_frame_resets_on_state_change();
    test_update_no_crash_without_skin();
    std::cout << "\nAll AnimationController tests passed.\n";
    return 0;
}
