#include <gtest/gtest.h>
#include "gameplay/PhaseConfig.h"

#ifndef TEST_ASSETS_DIR
#define TEST_ASSETS_DIR "."
#endif

static std::string testJson() {
    return TEST_ASSETS_DIR "/assets/config/fases.json";
}

// ── Loading ────────────────────────────────────────────────────────

TEST(PhaseConfigTest, LoadValidJson) {
    PhaseConfig config;
    EXPECT_TRUE(config.loadFromFile(testJson()));
}

TEST(PhaseConfigTest, LoadNonExistentFileReturnsFalse) {
    PhaseConfig config;
    EXPECT_FALSE(config.loadFromFile("nonexistent/fases.json"));
}

TEST(PhaseConfigTest, SizeReturnsSevenPhases) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    EXPECT_EQ(config.size(), 7);
}

// ── Phase data access ──────────────────────────────────────────────

TEST(PhaseConfigTest, GetPhase1Data) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    const auto& p = config.getPhase(0);
    EXPECT_EQ(p.id, 1);
    EXPECT_EQ(p.enemyCount, 2);
    EXPECT_FALSE(p.hasBoss);
    EXPECT_FLOAT_EQ(p.groundY, 900.0f);
    EXPECT_NE(p.background.find("fase1_patio"), std::string::npos);
}

TEST(PhaseConfigTest, GetPhase2DataHasBossRato) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    const auto& p = config.getPhase(1);
    EXPECT_EQ(p.id, 2);
    EXPECT_EQ(p.enemyCount, 3);
    EXPECT_TRUE(p.hasBoss);
    EXPECT_EQ(p.bossType, "rato");
    EXPECT_NE(p.background.find("fase2_bandeijao"), std::string::npos);
}

TEST(PhaseConfigTest, GetPhase3Data) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    const auto& p = config.getPhase(2);
    EXPECT_EQ(p.id, 3);
    EXPECT_EQ(p.enemyCount, 4);
    EXPECT_FALSE(p.hasBoss);
    EXPECT_NE(p.background.find("fase3_interusp"), std::string::npos);
}

TEST(PhaseConfigTest, GetPhase4DataHasBossMandrake) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    const auto& p = config.getPhase(3);
    EXPECT_EQ(p.id, 4);
    EXPECT_EQ(p.enemyCount, 3);
    EXPECT_TRUE(p.hasBoss);
    EXPECT_EQ(p.bossType, "mandrake");
    EXPECT_NE(p.background.find("fase4_busao"), std::string::npos);
}

TEST(PhaseConfigTest, GetPhase5DataHasBossPeru) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    const auto& p = config.getPhase(4);
    EXPECT_EQ(p.id, 5);
    EXPECT_EQ(p.enemyCount, 4);
    EXPECT_TRUE(p.hasBoss);
    EXPECT_EQ(p.bossType, "peru");
    EXPECT_NE(p.background.find("fase5_sanfran"), std::string::npos);
}

TEST(PhaseConfigTest, GetPhase6Data) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    const auto& p = config.getPhase(5);
    EXPECT_EQ(p.id, 6);
    EXPECT_EQ(p.enemyCount, 5);
    EXPECT_FALSE(p.hasBoss);
    EXPECT_NE(p.background.find("fase6_biblioteca"), std::string::npos);
}

TEST(PhaseConfigTest, GetPhase7DataHasBossProfessor) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    const auto& p = config.getPhase(6);
    EXPECT_EQ(p.id, 7);
    EXPECT_EQ(p.enemyCount, 2);
    EXPECT_TRUE(p.hasBoss);
    EXPECT_EQ(p.bossType, "professor");
    EXPECT_FLOAT_EQ(p.groundY, 990.0f);
    EXPECT_NE(p.background.find("fase7_reitoria"), std::string::npos);
}

// ── Convenience methods ────────────────────────────────────────────

TEST(PhaseConfigTest, ConvenienceGetBackground) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    EXPECT_NE(config.getBackground(0).find("fase1_patio"), std::string::npos);
}

TEST(PhaseConfigTest, ConvenienceGetEnemyCount) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    EXPECT_EQ(config.getEnemyCount(0), 2);  // F1
    EXPECT_EQ(config.getEnemyCount(1), 3);  // F2
    EXPECT_EQ(config.getEnemyCount(2), 4);  // F3
    EXPECT_EQ(config.getEnemyCount(3), 3);  // F4
    EXPECT_EQ(config.getEnemyCount(4), 4);  // F5
    EXPECT_EQ(config.getEnemyCount(5), 5);  // F6
    EXPECT_EQ(config.getEnemyCount(6), 2);  // F7
}

TEST(PhaseConfigTest, ConvenienceHasBoss) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    EXPECT_FALSE(config.hasBoss(0));  // F1: no
    EXPECT_TRUE(config.hasBoss(1));   // F2: rato
    EXPECT_FALSE(config.hasBoss(2));  // F3: no
    EXPECT_TRUE(config.hasBoss(3));   // F4: mandrake
    EXPECT_TRUE(config.hasBoss(4));   // F5: peru
    EXPECT_FALSE(config.hasBoss(5));  // F6: no
    EXPECT_TRUE(config.hasBoss(6));   // F7: professor
}

TEST(PhaseConfigTest, ConvenienceGetBossType) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    EXPECT_EQ(config.getBossType(1), "rato");
    EXPECT_EQ(config.getBossType(3), "mandrake");
    EXPECT_EQ(config.getBossType(4), "peru");
    EXPECT_EQ(config.getBossType(6), "professor");
    EXPECT_TRUE(config.getBossType(0).empty());
}

// ── Out-of-range access ────────────────────────────────────────────

TEST(PhaseConfigTest, OutOfRangeReturnsEmptyPhase) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    const auto& p = config.getPhase(-1);
    EXPECT_EQ(p.id, 0);
    EXPECT_TRUE(p.background.empty());
    EXPECT_EQ(p.enemyCount, 0);
    EXPECT_FALSE(p.hasBoss);
    const auto& p2 = config.getPhase(99);
    EXPECT_EQ(p2.id, 0);
}

TEST(PhaseConfigTest, GetGroundYPerPhase) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    EXPECT_FLOAT_EQ(config.getGroundY(0), 900.0f);
    EXPECT_FLOAT_EQ(config.getGroundY(1), 900.0f);
    EXPECT_FLOAT_EQ(config.getGroundY(2), 900.0f);
    EXPECT_FLOAT_EQ(config.getGroundY(3), 900.0f);
    EXPECT_FLOAT_EQ(config.getGroundY(4), 900.0f);
    EXPECT_FLOAT_EQ(config.getGroundY(5), 900.0f);
    EXPECT_FLOAT_EQ(config.getGroundY(6), 990.0f);  // F7 reitoria
}

TEST(PhaseConfigTest, OutOfRangeConvenienceReturnsDefaults) {
    PhaseConfig config;
    ASSERT_TRUE(config.loadFromFile(testJson()));
    EXPECT_TRUE(config.getBackground(-1).empty());
    EXPECT_EQ(config.getEnemyCount(-1), 0);
    EXPECT_FALSE(config.hasBoss(99));
    EXPECT_TRUE(config.getBossType(99).empty());
    EXPECT_FLOAT_EQ(config.getGroundY(-1), 900.0f);
}
