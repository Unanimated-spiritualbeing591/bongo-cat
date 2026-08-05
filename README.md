# BongoCat — Developer README

C++17 desktop overlay application. Tracks keyboard and mouse input via WinAPI low-level hooks and renders animated sprite sheets in a per-pixel-alpha layered window.

## Requirements

- Windows 10+ (build target; overlay uses `UpdateLayeredWindow` + GDI+)
- CMake 3.20+
- MSVC 2022 (or any C++17-capable compiler with Win32 headers)
- GDI+ (ships with Windows, linked via `gdiplus.lib`)

## Build

```bat
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Output: `build/bin/BongoCat.exe`

### Run tests

```bat
cd build
ctest -C Release --output-on-failure
```

## Project layout

```
files/
├── CMakeLists.txt
├── include/
│   └── BongoTypes.h          — shared enums, structs (InputEvent, PawState, Frame, …)
├── src/
│   ├── main.cpp              — entry point, parses args, constructs BongoCat
│   ├── core/
│   │   ├── BongoCat.h/.cpp   — top-level app class; owns all subsystems
│   │   └── InputTracker.h/.cpp — WH_KEYBOARD_LL + WH_MOUSE_LL hooks
│   ├── animation/
│   │   ├── SpriteSheet.h/.cpp      — PNG sprite sheet loader (GDI+)
│   │   └── AnimationController.h/.cpp — frame timer, PawState → sheet mapping
│   ├── overlay/
│   │   ├── OverlayWindow.h/.cpp    — WS_EX_LAYERED transparent overlay
│   │   └── ChromaKey.h/.cpp        — per-pixel chroma-key pass
│   └── skins/
│       ├── SkinLoader.h/.cpp       — loads one skin from a directory
│       └── SkinManager.h/.cpp      — scans skins/, manages active skin
└── tests/
    ├── test_input.cpp         — InputTracker unit tests
    └── test_animation.cpp     — AnimationController unit tests
```

## Skin format

Each skin is a directory under `skins/` containing:

```
skins/default/
├── idle.png           # single frame, RGBA PNG
├── left_down.png      # 1+ horizontal frames
├── right_down.png
├── both_down.png
└── left_down.frames.txt   # optional; contains integer frame count
```

All PNGs must be power-of-2 width divisible by frame count. Height is unconstrained.

## Architecture

```
InputTracker (WinAPI hooks)
      │  InputEvent dispatch
      ▼
BongoCat::onInput()
      │  setPawState()
      ▼
AnimationController
      │  currentFrame()
      ▼
OverlayWindow::drawFrame()   ←── ChromaKey::applyTo()
      │  UpdateLayeredWindow
      ▼
Desktop compositing
```

The render tick runs at ~60 fps in the main message loop. Input hooks fire on the same thread (low-level hooks require a message pump, which the main loop provides).

## Configuration

`AppConfig` in `src/core/BongoCat.h`:

| Field | Default | Description |
|---|---|---|
| `skinPath` | `""` | Active skin directory |
| `windowX/Y` | `100, 100` | Initial position |
| `windowW/H` | `200, 200` | Window size (px) |
| `alwaysOnTop` | `true` | Stay above other windows |
| `chromaKey` | `false` | Enable chroma background |
| `chromaColor` | `#00FF00` | Chroma key color |
| `opacity` | `1.0` | Overall window opacity |
| `leftKey` | `'Z'` | Left paw virtual key |
| `rightKey` | `'X'` | Right paw virtual key |

## License

MIT — see `LICENSE`.
