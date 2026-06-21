# ── Unit-test executable ──────────────────────────────────────────────
add_executable(unit_tests
    test_engine.cpp
    test_asset_manager.cpp
    test_frame_config.cpp
    test_health_component.cpp
    test_lives_component.cpp
    test_ammo_component.cpp
    test_damage_config.cpp
    test_ui_components.cpp
    test_animation.cpp
    test_phase_config.cpp
)

target_link_libraries(unit_tests PRIVATE
    UspicioLib
    GTest::gtest_main
)

# Allow tests to locate asset files regardless of cwd.
target_compile_definitions(unit_tests PRIVATE
    TEST_ASSETS_DIR="${CMAKE_SOURCE_DIR}"
)

# Register with CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)
