#include "core/BongoCat.h"
#include <windows.h>
#include <cstdlib>
#include <string>

// Entry point for the BongoCat overlay application.
// Usage: BongoCat.exe [skin_path] [x] [y] [w] [h]
// All arguments are optional; defaults are set in AppConfig.

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int) {
    bongo::AppConfig cfg;

    // Parse command-line arguments passed via lpCmdLine
    int    argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argc >= 2) cfg.skinPath = argv[1];
    if (argc >= 3) cfg.windowX  = _wtoi(argv[2]);
    if (argc >= 4) cfg.windowY  = _wtoi(argv[3]);
    if (argc >= 5) cfg.windowW  = _wtoi(argv[4]);
    if (argc >= 6) cfg.windowH  = _wtoi(argv[5]);

    if (argv) LocalFree(argv);

    // If no skin path supplied, default to ./skins/default
    if (cfg.skinPath.empty()) {
        wchar_t exeDir[MAX_PATH]{};
        GetModuleFileNameW(nullptr, exeDir, MAX_PATH);
        std::wstring dir(exeDir);
        auto pos = dir.rfind(L'\\');
        if (pos != std::wstring::npos) dir.resize(pos);
        cfg.skinPath = dir + L"\\skins\\default";
    }

    bongo::BongoCat app;
    if (!app.init(cfg)) {
        MessageBoxW(nullptr,
                    L"Failed to initialize BongoCat.\n"
                    L"Make sure the skins directory exists and contains idle.png.",
                    L"BongoCat", MB_ICONERROR | MB_OK);
        return 1;
    }

    return app.run();
}
