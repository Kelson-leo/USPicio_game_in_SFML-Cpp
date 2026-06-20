#include <gtest/gtest.h>
#include "infrastructure/SfmlRenderer.h"
#include "infrastructure/SfmlInput.h"
#include "gameplay/Game.h"
#include <SFML/Graphics.hpp>

// ────────────────────────────────────────────────────────────────────
// SfmlRenderer unit tests
// ────────────────────────────────────────────────────────────────────

TEST(SfmlRendererTest, Instantiation) {
    infrastructure::SfmlRenderer renderer;
    // A freshly-constructed renderer has no window.
    EXPECT_FALSE(renderer.isOpen());
}

TEST(SfmlRendererTest, OpenAndCloseWindow) {
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open(400, 300, "Test Window"));
    EXPECT_TRUE(renderer.isOpen());
    renderer.close();
    EXPECT_FALSE(renderer.isOpen());
}

TEST(SfmlRendererTest, ClearDoesNotCrashWhenClosed) {
    infrastructure::SfmlRenderer renderer;
    // Clearing before open should not crash.
    EXPECT_NO_THROW(renderer.clear());
}

// ────────────────────────────────────────────────────────────────────
// SfmlInput unit tests
// ────────────────────────────────────────────────────────────────────

TEST(SfmlInputTest, InstantiationWithWindow) {
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open(200, 200, "Input Test"));
    infrastructure::SfmlInput input(renderer.getWindow());
    // A freshly opened window may already have some queued events
    // (e.g. resize). Just verify that polling does not crash.
    sf::Event event;
    EXPECT_NO_THROW(input.pollEvent(event));
    renderer.close();
}

// ────────────────────────────────────────────────────────────────────
// Game unit tests (without rendering)
// ────────────────────────────────────────────────────────────────────

TEST(GameTest, ConstructionDoesNotThrow) {
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open(100, 100, "Game Test"));
    infrastructure::SfmlInput input(renderer.getWindow());
    EXPECT_NO_THROW({
        gameplay::Game game(renderer, input);
    });
    renderer.close();
}
