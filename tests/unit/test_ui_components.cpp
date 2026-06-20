#include <gtest/gtest.h>
#include "core/HealthComponent.h"
#include "core/LivesComponent.h"
#include "core/AmmoComponent.h"
#include "infrastructure/HealthBar.h"
#include "infrastructure/LivesDisplay.h"
#include "infrastructure/AmmoDisplay.h"
#include "infrastructure/SfmlRenderer.h"
#include <SFML/Graphics/Texture.hpp>

// ────────────────────────────────────────────────────────────────────
// HealthBar tests
// ────────────────────────────────────────────────────────────────────

TEST(HealthBarTest, ConstructionDoesNotThrow) {
    core::HealthComponent health;
    EXPECT_NO_THROW({
        infrastructure::HealthBar bar(health);
    });
}

TEST(HealthBarTest, ConstructionWithCustomSize) {
    core::HealthComponent health;
    infrastructure::HealthBar bar(health, {200.0f, 15.0f});
    // Just verify it doesn't crash
    EXPECT_NO_THROW(bar.setPosition({50.0f, 50.0f}));
}

TEST(HealthBarTest, DrawDoesNotThrowWhenDead) {
    core::HealthComponent health;
    health.takeDamage(100);
    ASSERT_TRUE(health.isDead());

    infrastructure::HealthBar bar(health);
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({100, 100}, "Test HealthBar"));
    EXPECT_NO_THROW(bar.draw(renderer));
    renderer.close();
}

TEST(HealthBarTest, DrawDoesNotThrowWhenAlive) {
    core::HealthComponent health;
    health.takeDamage(30);

    infrastructure::HealthBar bar(health);
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({100, 100}, "Test HealthBar"));
    EXPECT_NO_THROW(bar.draw(renderer));
    renderer.close();
}

// ────────────────────────────────────────────────────────────────────
// LivesDisplay tests
// ────────────────────────────────────────────────────────────────────

TEST(LivesDisplayTest, ConstructionDoesNotThrow) {
    core::LivesComponent lives;
    sf::Texture tex;
    tex.create(1, 1);
    EXPECT_NO_THROW({
        infrastructure::LivesDisplay display(lives, tex);
    });
}

TEST(LivesDisplayTest, DrawDoesNotThrow) {
    core::LivesComponent lives;
    sf::Texture tex;
    tex.create(1, 1);

    infrastructure::LivesDisplay display(lives, tex);
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({100, 100}, "Test Lives"));
    EXPECT_NO_THROW(display.draw(renderer));
    renderer.close();
}

TEST(LivesDisplayTest, DrawWithZeroLivesDoesNotCrash) {
    core::LivesComponent lives;
    lives.currentLives = 0;

    sf::Texture tex;
    tex.create(1, 1);

    infrastructure::LivesDisplay display(lives, tex);
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({100, 100}, "Test Lives Zero"));
    EXPECT_NO_THROW(display.draw(renderer));
    renderer.close();
}

// ────────────────────────────────────────────────────────────────────
// AmmoDisplay tests
// ────────────────────────────────────────────────────────────────────

TEST(AmmoDisplayTest, ConstructionDoesNotThrow) {
    core::AmmoComponent ammo;
    sf::Texture tex;
    tex.create(1, 1);
    EXPECT_NO_THROW({
        infrastructure::AmmoDisplay display(ammo, tex);
    });
}

TEST(AmmoDisplayTest, DrawDoesNotThrow) {
    core::AmmoComponent ammo;
    sf::Texture tex;
    tex.create(1, 1);

    infrastructure::AmmoDisplay display(ammo, tex);
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({100, 100}, "Test Ammo"));
    EXPECT_NO_THROW(display.draw(renderer));
    renderer.close();
}

TEST(AmmoDisplayTest, DrawWithZeroAmmoDoesNotCrash) {
    core::AmmoComponent ammo;
    ammo.currentAmmo = 0;

    sf::Texture tex;
    tex.create(1, 1);

    infrastructure::AmmoDisplay display(ammo, tex);
    infrastructure::SfmlRenderer renderer;
    ASSERT_TRUE(renderer.open({100, 100}, "Test Ammo Zero"));
    EXPECT_NO_THROW(display.draw(renderer));
    renderer.close();
}
