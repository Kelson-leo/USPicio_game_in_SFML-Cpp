# ── Unit-test executable ──────────────────────────────────────────────
add_executable(unit_tests
    test_engine.cpp
    test_asset_manager.cpp
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
