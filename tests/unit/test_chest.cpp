#include <gtest/gtest.h>
#include <cmath>
#include "gameplay/Chest.h"
#include "gameplay/Player.h"
#include "infrastructure/FrameConfig.h"
#include <SFML/Graphics/Texture.hpp>

#ifndef TEST_ASSETS_DIR
#define TEST_ASSETS_DIR "."
#endif

static std::string testJson() {
    return TEST_ASSETS_DIR "/assets/config/frames.json";
}

// ────────────────────────────────────────────────────────────────────
// Chest fixture
// ────────────────────────────────────────────────────────────────────

class ChestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        m_tex = sf::Texture::create({500, 500}).value();
        m_config.loadFromFile(testJson());
        m_player = std::make_unique<gameplay::Player>(*m_tex, m_config);
        m_player->setGroundY(900.0f);

        m_chest = std::make_unique<gameplay::Chest>();
        m_chest->init(*m_tex, m_config, {200.0f, 820.0f}, 900.0f);
    }

    std::optional<sf::Texture> m_tex;
    infrastructure::FrameConfig      m_config;
    std::unique_ptr<gameplay::Player> m_player;
    std::unique_ptr<gameplay::Chest>  m_chest;
};

TEST_F(ChestFixture, StartsClosed) {
    EXPECT_FALSE(m_chest->isOpen());
}

TEST_F(ChestFixture, RestoresLivesWithCeilFull) {
    // Lose 2 lives: ceil(2 * 0.8) = ceil(1.6) = 2 → fully restored
    m_player->lives.currentLives = 3;
    m_player->setPosition({200.0f, 820.0f});  // touch the chest

    m_chest->update(*m_player);

    EXPECT_TRUE(m_chest->isOpen());
    EXPECT_EQ(m_player->lives.currentLives, 5);
}

TEST_F(ChestFixture, RestoresLivesWithCeilPartial) {
    // Lose 3 lives: ceil(3 * 0.8) = ceil(2.4) = 3 → fully restored
    m_player->lives.currentLives = 2;
    m_player->setPosition({200.0f, 820.0f});

    m_chest->update(*m_player);

    EXPECT_TRUE(m_chest->isOpen());
    EXPECT_EQ(m_player->lives.currentLives, 5);
}

TEST_F(ChestFixture, RestoresLivesWithCeilOne) {
    // Lose 1 life: ceil(1 * 0.8) = ceil(0.8) = 1
    // 4/5 lives = 80% is NOT below threshold, so trigger via low ammo
    m_player->lives.currentLives = 4;
    m_player->ammo.currentAmmo = 7;  // 70% < 80% threshold → opens
    m_player->setPosition({200.0f, 820.0f});

    m_chest->update(*m_player);

    EXPECT_TRUE(m_chest->isOpen());
    EXPECT_EQ(m_player->lives.currentLives, 5);  // ceil(1*0.8)=1 restored
}

TEST_F(ChestFixture, RestoresAmmoWithCeil) {
    // Lose 5 ammo: ceil(5 * 0.8) = 4 → 5 + 4 = 9
    m_player->ammo.currentAmmo = 5;
    m_player->setPosition({200.0f, 820.0f});

    m_chest->update(*m_player);

    EXPECT_TRUE(m_chest->isOpen());
    EXPECT_EQ(m_player->ammo.currentAmmo, 9);
}

TEST_F(ChestFixture, DoesNotOpenWhenFull) {
    // At 100% lives and ammo, chest should NOT open
    m_player->setPosition({200.0f, 820.0f});

    m_chest->update(*m_player);

    EXPECT_FALSE(m_chest->isOpen());
    EXPECT_EQ(m_player->lives.currentLives, 5);
    EXPECT_EQ(m_player->ammo.currentAmmo, 10);
}

TEST_F(ChestFixture, DoesNotOpenWhenAboveThreshold) {
    // 4/5 lives = 80% and 8/10 ammo = 80% — exactly at threshold, NOT below
    m_player->lives.currentLives = 4;
    m_player->ammo.currentAmmo = 8;
    m_player->setPosition({200.0f, 820.0f});

    m_chest->update(*m_player);

    EXPECT_FALSE(m_chest->isOpen());
}

TEST_F(ChestFixture, OpensWhenLivesBelowThreshold) {
    // 3/5 lives = 60% (< 80%), ammo full
    m_player->lives.currentLives = 3;
    m_player->ammo.currentAmmo = 10;
    m_player->setPosition({200.0f, 820.0f});

    m_chest->update(*m_player);

    EXPECT_TRUE(m_chest->isOpen());
    // ceil((5-3) * 0.8) = ceil(1.6) = 2 → 3 + 2 = 5
    EXPECT_EQ(m_player->lives.currentLives, 5);
}

TEST_F(ChestFixture, OpensWhenAmmoBelowThreshold) {
    // Lives full, ammo 7/10 = 70% (< 80%)
    m_player->lives.currentLives = 5;
    m_player->ammo.currentAmmo = 7;
    m_player->setPosition({200.0f, 820.0f});

    m_chest->update(*m_player);

    EXPECT_TRUE(m_chest->isOpen());
    // ceil((10-7) * 0.8) = ceil(2.4) = 3 → 7 + 3 = 10
    EXPECT_EQ(m_player->ammo.currentAmmo, 10);
}

TEST_F(ChestFixture, DoesNotReopen) {
    m_player->lives.currentLives = 3;
    m_player->setPosition({200.0f, 820.0f});

    m_chest->update(*m_player);  // opens, restores
    EXPECT_TRUE(m_chest->isOpen());

    // Lose lives again and touch again
    m_player->lives.currentLives = 2;
    m_chest->update(*m_player);  // should do nothing

    EXPECT_EQ(m_player->lives.currentLives, 2);  // unchanged
}

TEST_F(ChestFixture, NoOpWhenNotTouching) {
    m_player->lives.currentLives = 3;
    m_player->setPosition({500.0f, 820.0f});  // far from chest at x=200

    m_chest->update(*m_player);

    EXPECT_FALSE(m_chest->isOpen());
    EXPECT_EQ(m_player->lives.currentLives, 3);  // unchanged
}

TEST_F(ChestFixture, HealPercentMath) {
    EXPECT_FLOAT_EQ(gameplay::Chest::HEAL_PERCENT, 0.8f);

    // ceil(2 * 0.8) = ceil(1.6) = 2
    EXPECT_EQ(static_cast<int>(std::ceil(2 * gameplay::Chest::HEAL_PERCENT)), 2);
    // ceil(1 * 0.8) = ceil(0.8) = 1
    EXPECT_EQ(static_cast<int>(std::ceil(1 * gameplay::Chest::HEAL_PERCENT)), 1);
    // ceil(0 * 0.8) = ceil(0) = 0
    EXPECT_EQ(static_cast<int>(std::ceil(0 * gameplay::Chest::HEAL_PERCENT)), 0);
}

// ────────────────────────────────────────────────────────────────────
// LivesComponent::restoreLives tests
// ────────────────────────────────────────────────────────────────────

TEST(LivesComponentTest, RestoreLivesIncrements) {
    core::LivesComponent l;
    l.currentLives = 2;
    l.restoreLives(2);
    EXPECT_EQ(l.currentLives, 4);
}

TEST(LivesComponentTest, RestoreLivesDoesNotExceedMax) {
    core::LivesComponent l;
    l.currentLives = 4;
    l.restoreLives(3);
    EXPECT_EQ(l.currentLives, 5);
}

TEST(LivesComponentTest, RestoreZeroDoesNothing) {
    core::LivesComponent l;
    l.currentLives = 3;
    l.restoreLives(0);
    EXPECT_EQ(l.currentLives, 3);
}
