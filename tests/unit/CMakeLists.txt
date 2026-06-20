# ── Unit-test executable ──────────────────────────────────────────────
add_executable(unit_tests
    test_engine.cpp
)

target_link_libraries(unit_tests PRIVATE
    UspicioLib
    GTest::gtest_main
)

# Register with CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)
