cmake_minimum_required(VERSION 3.20)
project(UspicioGame VERSION 0.1.0 LANGUAGES CXX)

# ── C++ Standard ───────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Library sources (shared between desktop and WASM) ──────────────────
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

# ── Test sources ────────────────────────────────────────────────────────
set(TEST_SOURCES
    tests/unit/test_engine.cpp
    tests/unit/test_asset_manager.cpp
    tests/unit/test_frame_config.cpp
    tests/unit/test_health_component.cpp
    tests/unit/test_lives_component.cpp
    tests/unit/test_ammo_component.cpp
    tests/unit/test_damage_config.cpp
    tests/unit/test_ui_components.cpp
    tests/unit/test_animation.cpp
    tests/unit/test_phase_config.cpp
    tests/unit/test_chest.cpp
)

# ── VRSFML paths ───────────────────────────────────────────────────────
set(VRSFML_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/vrsfml)
set(VRSFML_INCLUDE ${VRSFML_ROOT}/include)

# ── Shared dependencies (both WASM and desktop) ───────────────────────
include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

if(EMSCRIPTEN)
    # ── WASM build ─────────────────────────────────────────────────────
    message(STATUS "Configuring for WASM (Emscripten)")

    set(VRSFML_WASM_INSTALL ${VRSFML_ROOT}/install)
    set(VRSFML_WASM_LIB ${VRSFML_WASM_INSTALL}/lib)

    # ── VRSFML imported targets (WASM) ──────────────────────────────
    add_library(SFML_System STATIC IMPORTED)
    set_target_properties(SFML_System PROPERTIES
        IMPORTED_LOCATION ${VRSFML_WASM_LIB}/libsfml-system-s.html
        INTERFACE_INCLUDE_DIRECTORIES ${VRSFML_INCLUDE}
        INTERFACE_COMPILE_DEFINITIONS "SFML_STATIC"
    )

    add_library(SDL3 STATIC IMPORTED)
    set_target_properties(SDL3 PROPERTIES
        IMPORTED_LOCATION ${VRSFML_WASM_LIB}/libSDL3.a
    )

    add_library(SFML_Window STATIC IMPORTED)
    set_target_properties(SFML_Window PROPERTIES
        IMPORTED_LOCATION ${VRSFML_WASM_LIB}/libsfml-window-s.html
        INTERFACE_INCLUDE_DIRECTORIES ${VRSFML_INCLUDE}
        INTERFACE_LINK_LIBRARIES "SFML_System;SDL3"
    )

    add_library(SFML_GLUtils STATIC IMPORTED)
    set_target_properties(SFML_GLUtils PROPERTIES
        IMPORTED_LOCATION ${VRSFML_WASM_LIB}/libsfml-glutils-s.html
        INTERFACE_INCLUDE_DIRECTORIES ${VRSFML_INCLUDE}
    )

    add_library(Freetype INTERFACE IMPORTED)
    set_target_properties(Freetype PROPERTIES
        INTERFACE_LINK_LIBRARIES "${VRSFML_WASM_LIB}/libfreetype.a"
    )

    add_library(SFML_Graphics STATIC IMPORTED)
    set_target_properties(SFML_Graphics PROPERTIES
        IMPORTED_LOCATION ${VRSFML_WASM_LIB}/libsfml-graphics-s.html
        INTERFACE_INCLUDE_DIRECTORIES ${VRSFML_INCLUDE}
        INTERFACE_LINK_LIBRARIES "SFML_Window;Freetype;SFML_GLUtils"
    )

    add_library(Vorbis INTERFACE IMPORTED)
    set_target_properties(Vorbis PROPERTIES
        INTERFACE_LINK_LIBRARIES "${VRSFML_WASM_LIB}/libvorbis.a;${VRSFML_WASM_LIB}/libvorbisenc.a;${VRSFML_WASM_LIB}/libvorbisfile.a"
    )
    add_library(Ogg INTERFACE IMPORTED)
    set_target_properties(Ogg PROPERTIES
        INTERFACE_LINK_LIBRARIES "${VRSFML_WASM_LIB}/libogg.a"
    )
    add_library(FLAC INTERFACE IMPORTED)
    set_target_properties(FLAC PROPERTIES
        INTERFACE_LINK_LIBRARIES "${VRSFML_WASM_LIB}/libFLAC.a"
    )

    add_library(SFML_Audio STATIC IMPORTED)
    set_target_properties(SFML_Audio PROPERTIES
        IMPORTED_LOCATION ${VRSFML_WASM_LIB}/libsfml-audio-s.html
        INTERFACE_INCLUDE_DIRECTORIES ${VRSFML_INCLUDE}
        INTERFACE_LINK_LIBRARIES "SFML_System;Vorbis;Ogg;FLAC"
    )

else()
    # ── Desktop build ───────────────────────────────────────────────────
    message(STATUS "Configuring for Desktop (native)")

    set(VRSFML_DESKTOP_LIB ${VRSFML_ROOT}/build-desktop/lib)

    # ── VRSFML imported targets (Desktop) ───────────────────────────
    # libbacktrace (needed by sfml-system)
    add_library(Backtrace INTERFACE IMPORTED)
    set_target_properties(Backtrace PROPERTIES
        INTERFACE_LINK_LIBRARIES "${VRSFML_ROOT}/build-desktop/extlibs/libbacktrace/libsfml-libbacktrace.a"
    )

    # System
    add_library(SFML_System STATIC IMPORTED)
    set_target_properties(SFML_System PROPERTIES
        IMPORTED_LOCATION ${VRSFML_DESKTOP_LIB}/libsfml-system-s.a
        INTERFACE_INCLUDE_DIRECTORIES ${VRSFML_INCLUDE}
        INTERFACE_COMPILE_DEFINITIONS "SFML_STATIC"
        INTERFACE_LINK_LIBRARIES "Backtrace"
    )

    # SDL3 (with X11 dependencies)
    add_library(SDL3 STATIC IMPORTED)
    set_target_properties(SDL3 PROPERTIES
        IMPORTED_LOCATION ${VRSFML_DESKTOP_LIB}/libSDL3.a
    )

    # GLUtils
    add_library(SFML_GLUtils STATIC IMPORTED)
    set_target_properties(SFML_GLUtils PROPERTIES
        IMPORTED_LOCATION ${VRSFML_DESKTOP_LIB}/libsfml-glutils-s.a
        INTERFACE_INCLUDE_DIRECTORIES ${VRSFML_INCLUDE}
    )

    # Window
    add_library(SFML_Window STATIC IMPORTED)
    set_target_properties(SFML_Window PROPERTIES
        IMPORTED_LOCATION ${VRSFML_DESKTOP_LIB}/libsfml-window-s.a
        INTERFACE_INCLUDE_DIRECTORIES ${VRSFML_INCLUDE}
        INTERFACE_LINK_LIBRARIES "SFML_System;SDL3;SFML_GLUtils"
    )
    # UDev needed by sfml-window on Linux
    find_library(UDEV_LIBRARY udev REQUIRED)
    target_link_libraries(SFML_Window INTERFACE ${UDEV_LIBRARY} dl)

    # Freetype
    add_library(Freetype INTERFACE IMPORTED)
    set_target_properties(Freetype PROPERTIES
        INTERFACE_LINK_LIBRARIES "${VRSFML_DESKTOP_LIB}/libfreetype.a"
    )

    # Graphics
    add_library(SFML_Graphics STATIC IMPORTED)
    set_target_properties(SFML_Graphics PROPERTIES
        IMPORTED_LOCATION ${VRSFML_DESKTOP_LIB}/libsfml-graphics-s.a
        INTERFACE_INCLUDE_DIRECTORIES ${VRSFML_INCLUDE}
        INTERFACE_LINK_LIBRARIES "SFML_Window;Freetype;SFML_GLUtils"
    )

    # Audio deps
    add_library(Vorbis INTERFACE IMPORTED)
    set_target_properties(Vorbis PROPERTIES
        INTERFACE_LINK_LIBRARIES "${VRSFML_DESKTOP_LIB}/libvorbis.a;${VRSFML_DESKTOP_LIB}/libvorbisenc.a;${VRSFML_DESKTOP_LIB}/libvorbisfile.a"
    )
    add_library(Ogg INTERFACE IMPORTED)
    set_target_properties(Ogg PROPERTIES
        INTERFACE_LINK_LIBRARIES "${VRSFML_DESKTOP_LIB}/libogg.a"
    )
    add_library(FLAC INTERFACE IMPORTED)
    set_target_properties(FLAC PROPERTIES
        INTERFACE_LINK_LIBRARIES "${VRSFML_DESKTOP_LIB}/libFLAC.a"
    )

    # Audio
    add_library(SFML_Audio STATIC IMPORTED)
    set_target_properties(SFML_Audio PROPERTIES
        IMPORTED_LOCATION ${VRSFML_DESKTOP_LIB}/libsfml-audio-s.a
        INTERFACE_INCLUDE_DIRECTORIES ${VRSFML_INCLUDE}
        INTERFACE_LINK_LIBRARIES "SFML_System;Vorbis;Ogg;FLAC"
    )

    # ── Link system libs needed by SDL3 on Linux ────────────────────
    find_package(Threads REQUIRED)
    find_package(X11 REQUIRED)
    target_link_libraries(SDL3 INTERFACE
        Threads::Threads
        ${X11_LIBRARIES}
        dl
    )
endif()

# ── Game Library ────────────────────────────────────────────────────────
add_library(UspicioLib STATIC ${LIB_SOURCES})

target_include_directories(UspicioLib PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

# Match VRSFML build settings (ABI compatibility)
# VRSFML was built with these enabled
target_compile_definitions(UspicioLib PUBLIC
    SFML_ENABLE_LIFETIME_TRACKING
    SFML_ENABLE_STACK_TRACES
)

# WASM: VRSFML WASM libs are Release-built (no SFML_DEBUG/assertFailure symbol).
# NDEBUG prevents SFML_DEBUG from being defined, avoiding link errors in Debug mode.
# Desktop: VRSFML is built locally with matching flags, unaffected.
if(EMSCRIPTEN)
    target_compile_definitions(UspicioLib PUBLIC NDEBUG)
endif()

target_link_libraries(UspicioLib PUBLIC
    SFML_Graphics
    SFML_Window
    SFML_System
    SFML_Audio
    nlohmann_json
)

# ── Executable ──────────────────────────────────────────────────────────
add_executable(UspicioGame src/main.cpp)
target_link_libraries(UspicioGame PRIVATE UspicioLib)

target_compile_definitions(UspicioGame PRIVATE
    ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets/"
)

if(EMSCRIPTEN)
    set_target_properties(UspicioGame PROPERTIES
        SUFFIX ".html"
        LINK_FLAGS "--shell-file ${VRSFML_ROOT}/emscripten/shell.html --pre-js ${VRSFML_ROOT}/emscripten/pre.js --preload-file ${CMAKE_SOURCE_DIR}/assets@/assets -s USE_SDL=0 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s GL_EXPLICIT_UNIFORM_LOCATION=1 -s ALLOW_MEMORY_GROWTH=1 -pthread -s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE_STRICT=0 -s EXIT_RUNTIME=1 -s FORCE_FILESYSTEM=1 -s FETCH=1 -s STACK_SIZE=4mb -s TOTAL_MEMORY=128mb -s ASYNCIFY=1 -sASYNCIFY_IGNORE_INDIRECT=1 -s ASSERTIONS=1 -s EXPORTED_RUNTIME_METHODS=HEAPU32,HEAP8,getValue,setValue,requestFullscreen -Wno-pthreads-mem-growth"
    )
endif()

# Copy assets to build directory
add_custom_command(TARGET UspicioGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:UspicioGame>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${VRSFML_ROOT}/emscripten/coi-serviceworker.js
    $<TARGET_FILE_DIR:UspicioGame>/coi-serviceworker.js
)

# ── Unit Tests ──────────────────────────────────────────────────────────
add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE UspicioLib gtest gtest_main)

target_compile_definitions(unit_tests PRIVATE
    TEST_ASSETS_DIR="${CMAKE_SOURCE_DIR}"
)

if(EMSCRIPTEN)
    set_target_properties(unit_tests PROPERTIES
        SUFFIX ".html"
        LINK_FLAGS "-s USE_SDL=0 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1"
    )
endif()

# Copy test data to build dir
add_custom_command(TARGET unit_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_CURRENT_BINARY_DIR}/assets
)
