#include <gtest/gtest.h>
#include "core/HealthComponent.h"

TEST(HealthComponentTest, InitialValues) {
    core::HealthComponent h;
    EXPECT_EQ(h.maxHP, 100);
    EXPECT_EQ(h.currentHP, 100);
    EXPECT_FALSE(h.isDead());
}

TEST(HealthComponentTest, TakeDamage) {
    core::HealthComponent h;
    h.takeDamage(30);
    EXPECT_EQ(h.currentHP, 70);
    EXPECT_FALSE(h.isDead());
}

TEST(HealthComponentTest, TakeDamageWithDefense70PercentReduction) {
    core::HealthComponent h;
    h.takeDamage(30, true);
    // ceil(30 * 0.3) = 9 → 100 - 9 = 91
    EXPECT_EQ(h.currentHP, 91);
}

TEST(HealthComponentTest, TakeDamageWithDefense30PercentTaken) {
    core::HealthComponent h;
    h.takeDamage(20, true);
    // ceil(20 * 0.3) = 6 → 100 - 6 = 94
    EXPECT_EQ(h.currentHP, 94);
}

TEST(HealthComponentTest, TakeDamageWithDefense10Damage) {
    core::HealthComponent h;
    h.takeDamage(10, true);
    // ceil(10 * 0.3) = 3 → 100 - 3 = 97
    EXPECT_EQ(h.currentHP, 97);
}

TEST(HealthComponentTest, DefenseFloorIsOne) {
    core::HealthComponent h;
    h.takeDamage(1, true);
    EXPECT_EQ(h.currentHP, 99);  // 100 - max(1, 1/2=0) = 100 - 1
}

TEST(HealthComponentTest, DamageCannotGoBelowZero) {
    core::HealthComponent h;
    h.takeDamage(200);
    EXPECT_EQ(h.currentHP, 0);
}

TEST(HealthComponentTest, IsDeadWhenZero) {
    core::HealthComponent h;
    h.takeDamage(100);
    EXPECT_TRUE(h.isDead());
}

TEST(HealthComponentTest, HealRestoresHP) {
    core::HealthComponent h;
    h.takeDamage(50);
    h.heal(20);
    EXPECT_EQ(h.currentHP, 70);
}

TEST(HealthComponentTest, HealDoesNotExceedMax) {
    core::HealthComponent h;
    h.takeDamage(10);
    h.heal(50);
    EXPECT_EQ(h.currentHP, h.maxHP);
}

TEST(HealthComponentTest, HealDeadCharacter) {
    core::HealthComponent h;
    h.takeDamage(100);
    ASSERT_TRUE(h.isDead());
    h.heal(30);
    EXPECT_EQ(h.currentHP, 30);
    EXPECT_FALSE(h.isDead());
}
