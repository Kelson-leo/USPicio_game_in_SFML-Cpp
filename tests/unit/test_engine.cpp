#include <gtest/gtest.h>
#include "core/IRenderer.h"
#include "core/IInputHandler.h"
#include "core/PhysicsConstants.h"
#include "infrastructure/SfmlRenderer.h"
#include "infrastructure/SfmlInput.h"
#include "gameplay/Game.h"

// ────────────────────────────────────────────────────────────────────
// SfmlRenderer unit tests
// ────────────────────────────────────────────────────────────────────

TEST(SfmlRendererTest, Instantiation) {
    infrastructure::SfmlRenderer renderer;
    EXPECT_FALSE(renderer.isOpen());
}

TEST(SfmlRendererTest, OpenAndCloseWindow) {
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({400, 300}, "Test Window"));
    EXPECT_TRUE(renderer.isOpen());

    const auto sz = renderer.getSize();
    EXPECT_EQ(sz.x, 400u);
    EXPECT_EQ(sz.y, 300u);

    renderer.close();
    EXPECT_FALSE(renderer.isOpen());
}

TEST(SfmlRendererTest, ClearDoesNotCrashWhenClosed) {
    infrastructure::SfmlRenderer renderer;
    EXPECT_NO_THROW(renderer.clear(core::Color::Blue()));
}

TEST(SfmlRendererTest, ClearWithCoreColor) {
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({200, 200}, "Color Test"));
    EXPECT_NO_THROW(renderer.clear(core::Color{100, 150, 200}));
    renderer.close();
}

// ────────────────────────────────────────────────────────────────────
// SfmlInput unit tests
// ────────────────────────────────────────────────────────────────────

TEST(SfmlInputTest, PollEventDoesNotCrash) {
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({200, 200}, "Input Test"));
    infrastructure::SfmlInput input(renderer.getSfmlWindow());
    core::Event event;
    // A freshly-opened window may have events; just check it doesn't crash.
    EXPECT_NO_THROW(input.pollEvent(event));
    renderer.close();
}

TEST(SfmlInputTest, IsKeyPressedDoesNotCrash) {
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({200, 200}, "Key Test"));
    infrastructure::SfmlInput input(renderer.getSfmlWindow());
    EXPECT_NO_THROW(input.isKeyPressed(core::KeyCode::Space));
    renderer.close();
}

// ────────────────────────────────────────────────────────────────────
// Game unit tests
// ────────────────────────────────────────────────────────────────────

TEST(GameTest, ConstructionDoesNotThrow) {
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({100, 100}, "Game Test"));
    infrastructure::SfmlInput input(renderer.getSfmlWindow());
    EXPECT_NO_THROW({
        gameplay::Game game(renderer, input);
    });
    renderer.close();
}

// ────────────────────────────────────────────────────────────────────
// Core type tests (no SFML dependency)
// ────────────────────────────────────────────────────────────────────

TEST(CoreTypesTest, ColorDefaults) {
    const core::Color c;
    EXPECT_EQ(c.r, 0);
    EXPECT_EQ(c.g, 0);
    EXPECT_EQ(c.b, 0);
    EXPECT_EQ(c.a, 255);
}

TEST(CoreTypesTest, ColorStaticFactories) {
    EXPECT_EQ(core::Color::Black().r, 0);
    EXPECT_EQ(core::Color::White().r, 255);
    EXPECT_EQ(core::Color::Red().r,   255);
    EXPECT_EQ(core::Color::Red().g,   0);
}

TEST(CoreTypesTest, EventDefaults) {
    const core::Event e;
    EXPECT_EQ(e.type, core::EventType::KeyPressed);
    EXPECT_EQ(e.key,  core::KeyCode::Unknown);
}

TEST(CoreTypesTest, PhysicsConstantsArePositive) {
    EXPECT_GT(core::GROUND_Y,      0.0f);
    EXPECT_GT(core::GROUND_HEIGHT, 0.0f);
    EXPECT_GT(core::GRAVITY,       0.0f);
    EXPECT_LT(core::JUMP_SPEED,    0.0f); // negative = upward
}
