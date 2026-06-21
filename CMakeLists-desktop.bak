cmake_minimum_required(VERSION 3.20)
project(UspicioGame VERSION 0.1.0 LANGUAGES CXX)

# ── C++17 ─────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── SFML 2.6 ─────────────────────────────────────────────────────────
find_package(SFML 2.6 REQUIRED graphics window system)

# ── Google Test (FetchContent) ────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

# ── nlohmann/json (header-only JSON parser) ─────────────────────────
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

# ── Library (game engine, shared between exe and tests) ───────────────
set(LIB_SOURCES
    src/core/DamageConfig.cpp
    src/gameplay/Capivara.cpp
    src/gameplay/Chest.cpp
    src/gameplay/Game.cpp
    src/gameplay/Level.cpp
    src/gameplay/Player.cpp
    src/gameplay/Professor.cpp
    src/gameplay/Projectile.cpp
    src/gameplay/Boss.cpp
    src/gameplay/Rato.cpp
    src/gameplay/Mandrake.cpp
    src/gameplay/Peru.cpp
    src/gameplay/PhaseConfig.cpp
    src/infrastructure/AmmoDisplay.cpp
    src/infrastructure/AssetManager.cpp
    src/infrastructure/AudioManager.cpp
    src/infrastructure/FrameConfig.cpp
    src/infrastructure/HealthBar.cpp
    src/infrastructure/LivesDisplay.cpp
    src/infrastructure/SfmlInput.cpp
    src/infrastructure/SfmlRenderer.cpp
    src/infrastructure/SfmlSprite.cpp
    src/infrastructure/SfmlText.cpp
    src/infrastructure/SfmlTextureLoader.cpp
)

add_library(UspicioLib STATIC ${LIB_SOURCES})

target_include_directories(UspicioLib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(UspicioLib PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    nlohmann_json
)

# ── Executable ────────────────────────────────────────────────────────
add_executable(UspicioGame src/main.cpp)

target_link_libraries(UspicioGame PRIVATE UspicioLib)

# Copy assets to build directory so the executable finds them
# regardless of the working directory at runtime.
add_custom_command(TARGET UspicioGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:UspicioGame>/assets
)

# Assets directory relative to executable
target_compile_definitions(UspicioGame PRIVATE
    ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets/"
)

# ── Tests ─────────────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests)
