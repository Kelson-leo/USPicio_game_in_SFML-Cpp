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

# ── Library (game engine, shared between exe and tests) ───────────────
set(LIB_SOURCES
    src/gameplay/Game.cpp
    src/infrastructure/SfmlRenderer.cpp
    src/infrastructure/SfmlInput.cpp
)

add_library(UspicioLib STATIC ${LIB_SOURCES})

target_include_directories(UspicioLib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(UspicioLib PUBLIC
    sfml-graphics
    sfml-window
    sfml-system
)

# ── Executable ────────────────────────────────────────────────────────
add_executable(UspicioGame src/main.cpp)

target_link_libraries(UspicioGame PRIVATE UspicioLib)

# Assets directory relative to executable
target_compile_definitions(UspicioGame PRIVATE
    ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets/"
)

# ── Tests ─────────────────────────────────────────────────────────────
enable_testing()
add_subdirectory(tests)
