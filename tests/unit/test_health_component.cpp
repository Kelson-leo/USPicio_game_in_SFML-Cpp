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

TEST(HealthComponentTest, TakeDamageWithDefenseReduces80Percent) {
    core::HealthComponent h;
    h.takeDamage(30, true);
    EXPECT_EQ(h.currentHP, 94);  // 100 - (30/5) = 100 - 6
}

TEST(HealthComponentTest, TakeDamageWithDefense80Percent) {
    core::HealthComponent h;
    h.takeDamage(20, true);
    EXPECT_EQ(h.currentHP, 96);  // 100 - (20/5) = 100 - 4
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
