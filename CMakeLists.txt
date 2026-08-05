cmake_minimum_required(VERSION 3.20)
project(BongoCat VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Output directories ----
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---- Source files ----
set(BONGO_SOURCES
    src/core/BongoCat.cpp
    src/core/InputTracker.cpp
    src/animation/SpriteSheet.cpp
    src/animation/AnimationController.cpp
    src/overlay/OverlayWindow.cpp
    src/overlay/ChromaKey.cpp
    src/skins/SkinLoader.cpp
    src/skins/SkinManager.cpp
)

set(BONGO_HEADERS
    include/BongoTypes.h
    src/core/BongoCat.h
    src/core/InputTracker.h
    src/animation/SpriteSheet.h
    src/animation/AnimationController.h
    src/overlay/OverlayWindow.h
    src/overlay/ChromaKey.h
    src/skins/SkinLoader.h
    src/skins/SkinManager.h
)

# ---- Main executable ----
add_executable(BongoCat WIN32
    src/main.cpp
    ${BONGO_SOURCES}
    ${BONGO_HEADERS}
)

target_include_directories(BongoCat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# GDI+ for PNG decoding in SpriteSheet
target_link_libraries(BongoCat PRIVATE gdiplus gdi32 user32 kernel32)

# Windows subsystem - no console window in release builds
if(MSVC)
    target_compile_options(BongoCat PRIVATE /W4 /WX /MP /utf-8)
    target_compile_definitions(BongoCat PRIVATE _UNICODE UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
    # Enable whole-program optimization for Release
    set_target_properties(BongoCat PROPERTIES
        LINK_FLAGS_RELEASE "/LTCG"
    )
endif()

# ---- Test executables ----
enable_testing()

add_executable(test_input tests/test_input.cpp
    src/core/InputTracker.cpp)
target_include_directories(test_input PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(test_input PRIVATE user32 kernel32)
if(MSVC)
    target_compile_options(test_input PRIVATE /W4 /utf-8)
    target_compile_definitions(test_input PRIVATE _UNICODE UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()
add_test(NAME InputTrackerTests COMMAND test_input)

add_executable(test_animation tests/test_animation.cpp
    src/animation/SpriteSheet.cpp
    src/animation/AnimationController.cpp
    src/skins/SkinLoader.cpp
    src/skins/SkinManager.cpp)
target_include_directories(test_animation PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(test_animation PRIVATE gdiplus gdi32 user32 kernel32)
if(MSVC)
    target_compile_options(test_animation PRIVATE /W4 /utf-8)
    target_compile_definitions(test_animation PRIVATE _UNICODE UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()
add_test(NAME AnimationControllerTests COMMAND test_animation)

# ---- Install ----
install(TARGETS BongoCat RUNTIME DESTINATION .)
install(DIRECTORY skins/ DESTINATION skins OPTIONAL)
install(FILES LICENSE DESTINATION .)
