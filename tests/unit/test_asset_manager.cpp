#include <gtest/gtest.h>
#include "infrastructure/AssetManager.h"

// ────────────────────────────────────────────────────────────────────
// AssetManager unit tests (TDD — written before implementation)
// ────────────────────────────────────────────────────────────────────

// CMake passes the source dir so tests can find assets regardless of
// the cwd where ctest was launched.
#ifndef TEST_ASSETS_DIR
#define TEST_ASSETS_DIR "."
#endif

static const char* testPng() {
    return TEST_ASSETS_DIR "/tests/unit/test_data/red_pixel.png";
}

TEST(AssetManagerTest, LoadExistingTexture) {
    infrastructure::AssetManager& am = infrastructure::AssetManager::instance();

    EXPECT_TRUE(am.loadTexture("test_red", testPng()));
    const sf::Texture& tex = am.getTexture("test_red");
    EXPECT_EQ(tex.getSize().x, 1u);
    EXPECT_EQ(tex.getSize().y, 1u);
}

TEST(AssetManagerTest, LoadNonExistentTextureReturnsFalse) {
    infrastructure::AssetManager& am = infrastructure::AssetManager::instance();
    EXPECT_FALSE(am.loadTexture("does_not_exist", "nonexistent/file.png"));
}

TEST(AssetManagerTest, GetInvalidIdReturnsFallback) {
    infrastructure::AssetManager& am = infrastructure::AssetManager::instance();
    const sf::Texture& tex = am.getTexture("invalid_id");
    // Fallback is non-empty (placeholder).
    EXPECT_GE(tex.getSize().x, 1u);
    EXPECT_GE(tex.getSize().y, 1u);
}

TEST(AssetManagerTest, ClearUnloadsAllTextures) {
    infrastructure::AssetManager& am = infrastructure::AssetManager::instance();
    ASSERT_TRUE(am.loadTexture("temp", testPng()));

    am.clear();
    // After clear, "temp" should return fallback (still non-empty).
    const sf::Texture& tex = am.getTexture("temp");
    EXPECT_GE(tex.getSize().x, 1u);
}

TEST(AssetManagerTest, ReloadAfterClearWorks) {
    infrastructure::AssetManager& am = infrastructure::AssetManager::instance();
    am.clear();
    ASSERT_TRUE(am.loadTexture("reload_test", testPng()));
    const sf::Texture& tex = am.getTexture("reload_test");
    EXPECT_EQ(tex.getSize().x, 1u);
}
