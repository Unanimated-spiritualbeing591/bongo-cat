#include <cassert>
#include <iostream>
#include "../src/core/InputTracker.h"

// Minimal unit tests for InputTracker — runs without a message loop.
// Tests only the state-tracking logic; hook installation requires a real desktop session.

using namespace bongo;

static void test_initial_state() {
    InputTracker tracker;
    for (int vk = 0; vk < 256; ++vk) {
        assert(!tracker.isKeyDown(vk) && "all keys must be up on init");
    }
    assert(!tracker.isMouseButtonDown(MouseButton::Left));
    assert(!tracker.isMouseButtonDown(MouseButton::Right));
    assert(!tracker.isMouseButtonDown(MouseButton::Middle));
    std::cout << "[PASS] initial_state\n";
}

static void test_vk_out_of_range() {
    InputTracker tracker;
    // VK >= 256 must not crash and must return false
    assert(!tracker.isKeyDown(256));
    assert(!tracker.isKeyDown(0xFFFF));
    std::cout << "[PASS] vk_out_of_range\n";
}

static void test_callback_registration() {
    InputTracker tracker;
    int callCount = 0;
    tracker.addCallback([&](const InputEvent&) { ++callCount; });
    // Can't fire hooks without install, but clearing must not crash
    tracker.clearCallbacks();
    assert(callCount == 0);
    std::cout << "[PASS] callback_registration\n";
}

static void test_multiple_callbacks() {
    InputTracker tracker;
    int a = 0, b = 0;
    tracker.addCallback([&](const InputEvent&) { ++a; });
    tracker.addCallback([&](const InputEvent&) { ++b; });
    tracker.clearCallbacks();
    // After clear, adding a new one works
    int c = 0;
    tracker.addCallback([&](const InputEvent&) { ++c; });
    std::cout << "[PASS] multiple_callbacks\n";
}

int main() {
    test_initial_state();
    test_vk_out_of_range();
    test_callback_registration();
    test_multiple_callbacks();
    std::cout << "\nAll InputTracker tests passed.\n";
    return 0;
}
