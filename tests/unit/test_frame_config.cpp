#include <gtest/gtest.h>
#include "infrastructure/FrameConfig.h"
#include <SFML/Graphics/Rect.hpp>

#ifndef TEST_ASSETS_DIR
#define TEST_ASSETS_DIR "."
#endif

// ────────────────────────────────────────────────────────────────────
// FrameConfig unit tests (TDD — written before implementation)
// ────────────────────────────────────────────────────────────────────

static std::string testJson() {
    return TEST_ASSETS_DIR "/assets/config/frames.json";
}

// ── Loading ────────────────────────────────────────────────────────

TEST(FrameConfigTest, LoadValidJson) {
    infrastructure::FrameConfig config;
    EXPECT_TRUE(config.loadFromFile(testJson()));
}

TEST(FrameConfigTest, LoadNonExistentFileReturnsFalse) {
    infrastructure::FrameConfig config;
    EXPECT_FALSE(config.loadFromFile("nonexistent/frames.json"));
}

// ── Querying frames by character + animation ───────────────────────

TEST(FrameConfigTest, GetPlayerIdleFrame) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    auto rect = config.getFrame("player", "idle_right", 0);
    EXPECT_EQ(rect.left,   75);
    EXPECT_EQ(rect.top,    19);
    EXPECT_EQ(rect.width,  40);
    EXPECT_EQ(rect.height, 95);
}

TEST(FrameConfigTest, GetPlayerWalkFrames) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    auto rect0 = config.getFrame("player", "walk_right", 0);
    EXPECT_EQ(rect0.left,   132);
    EXPECT_EQ(rect0.width,  61);

    auto rect1 = config.getFrame("player", "walk_right", 1);
    EXPECT_EQ(rect1.left,   207);
    EXPECT_EQ(rect1.width,  73);
}

TEST(FrameConfigTest, PlayerPunchWiderThanIdle) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    auto idle  = config.getFrame("player", "idle_right", 0);
    auto punch = config.getFrame("player", "punch_right", 0);
    EXPECT_GT(punch.width, idle.width);   // 61 > 40
}

TEST(FrameConfigTest, PlayerThrowEvenWider) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    auto punch = config.getFrame("player", "punch_right", 0);
    auto thr0  = config.getFrame("player", "throw_right", 0);
    EXPECT_GT(thr0.width, punch.width);   // 73 > 61
}

TEST(FrameConfigTest, PlayerDefendWidthBetween) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    auto defend = config.getFrame("player", "defend_right", 0);
    EXPECT_EQ(defend.width, 60);
}

// ── Capivara ───────────────────────────────────────────────────────

TEST(FrameConfigTest, CapivaraFrames) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    EXPECT_EQ(config.getFrame("capivara", "idle", 0).width,  64);
    EXPECT_EQ(config.getFrame("capivara", "hurt", 0).width,  64);
    EXPECT_EQ(config.getFrame("capivara", "dead", 0).width,  64);
}

// ── Professor ──────────────────────────────────────────────────────

TEST(FrameConfigTest, ProfessorFramesLarger) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    auto idle  = config.getFrame("professor", "idle", 0);
    EXPECT_EQ(idle.width,  80);
    EXPECT_EQ(idle.height, 80);

    auto atk0 = config.getFrame("professor", "attack", 0);
    EXPECT_EQ(atk0.width, 100);

    auto atk1 = config.getFrame("professor", "attack", 1);
    EXPECT_EQ(atk1.left, 180);
}

// ── Missing data ───────────────────────────────────────────────────

TEST(FrameConfigTest, MissingCharacterReturnsEmptyFrame) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    auto rect = config.getFrame("nonexistent", "idle", 0);
    EXPECT_EQ(rect.width,  0);
    EXPECT_EQ(rect.height, 0);
}

TEST(FrameConfigTest, MissingAnimationReturnsEmptyFrame) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    auto rect = config.getFrame("player", "nonexistent", 0);
    EXPECT_EQ(rect.width,  0);
    EXPECT_EQ(rect.height, 0);
}

TEST(FrameConfigTest, OutOfRangeFrameIndexReturnsLastFrame) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    // Player idle_right has 1 frame; index 99 should return the last valid
    // frame (index 0).
    auto rect = config.getFrame("player", "idle_right", 99);
    EXPECT_EQ(rect.width, 40);
}

// ── Frame count ────────────────────────────────────────────────────

TEST(FrameConfigTest, FrameCount) {
    infrastructure::FrameConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));

    EXPECT_EQ(config.frameCount("player", "idle_right"),   1);
    EXPECT_EQ(config.frameCount("player", "walk_right"),   4);
    EXPECT_EQ(config.frameCount("player", "throw_right"),  1);
    EXPECT_EQ(config.frameCount("player", "punch_right"),  3);
    EXPECT_EQ(config.frameCount("professor", "attack"),    2);
    EXPECT_EQ(config.frameCount("nonexistent", "idle"),    0);
    EXPECT_EQ(config.frameCount("player", "nonexistent"),  0);
}
