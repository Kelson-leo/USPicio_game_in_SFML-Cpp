#include <gtest/gtest.h>
#include "core/DamageConfig.h"

TEST(DamageConfigTest, PunchVsCapivara) {
    core::DamageConfig cfg;
    EXPECT_EQ(cfg.getDamage(core::AttackType::Punch, core::EntityType::Capivara), 10);
}

TEST(DamageConfigTest, PunchVsProfessor) {
    core::DamageConfig cfg;
    EXPECT_EQ(cfg.getDamage(core::AttackType::Punch, core::EntityType::Professor), 5);
}

TEST(DamageConfigTest, ThrowVsCapivara) {
    core::DamageConfig cfg;
    EXPECT_EQ(cfg.getDamage(core::AttackType::Throw, core::EntityType::Capivara), 20);
}

TEST(DamageConfigTest, ThrowVsProfessor) {
    core::DamageConfig cfg;
    EXPECT_EQ(cfg.getDamage(core::AttackType::Throw, core::EntityType::Professor), 15);
}

TEST(DamageConfigTest, EnemyTouchVsPlayer) {
    core::DamageConfig cfg;
    EXPECT_EQ(cfg.getDamage(core::AttackType::EnemyTouch, core::EntityType::Player), 8);
}

TEST(DamageConfigTest, BossProjectileVsPlayer) {
    core::DamageConfig cfg;
    EXPECT_EQ(cfg.getDamage(core::AttackType::BossProjectile, core::EntityType::Player), 12);
}

TEST(DamageConfigTest, UnknownCombinationReturnsZero) {
    core::DamageConfig cfg;
    // Punch vs Player is undefined in the default config
    EXPECT_EQ(cfg.getDamage(core::AttackType::Punch, core::EntityType::Player), 0);
}

TEST(DamageConfigTest, SetDamageUpdatesValue) {
    core::DamageConfig cfg;
    cfg.setDamage(core::AttackType::Punch, core::EntityType::Capivara, 15);
    EXPECT_EQ(cfg.getDamage(core::AttackType::Punch, core::EntityType::Capivara), 15);
}

TEST(DamageConfigTest, SetDamageNewCombination) {
    core::DamageConfig cfg;
    cfg.setDamage(core::AttackType::EnemyTouch, core::EntityType::Capivara, 7);
    EXPECT_EQ(cfg.getDamage(core::AttackType::EnemyTouch, core::EntityType::Capivara), 7);
}
