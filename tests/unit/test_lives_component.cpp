#include <gtest/gtest.h>
#include "core/LivesComponent.h"

TEST(LivesComponentTest, InitialValues) {
    core::LivesComponent l;
    EXPECT_EQ(l.maxLives, 5);
    EXPECT_EQ(l.currentLives, 5);
    EXPECT_FALSE(l.isGameOver());
}

TEST(LivesComponentTest, LoseLifeDecrements) {
    core::LivesComponent l;
    l.loseLife();
    EXPECT_EQ(l.currentLives, 4);
    EXPECT_FALSE(l.isGameOver());
}

TEST(LivesComponentTest, LoseAllLivesTriggersGameOver) {
    core::LivesComponent l;
    l.loseLife();
    l.loseLife();
    l.loseLife();
    l.loseLife();
    l.loseLife();
    EXPECT_EQ(l.currentLives, 0);
    EXPECT_TRUE(l.isGameOver());
}

TEST(LivesComponentTest, LoseLifeBeyondZeroStaysAtZero) {
    core::LivesComponent l;
    for (int i = 0; i < 10; ++i) {
        l.loseLife();
    }
    EXPECT_EQ(l.currentLives, 0);
}

TEST(LivesComponentTest, IsGameOverWhenZeroLives) {
    core::LivesComponent l;
    l.currentLives = 0;
    EXPECT_TRUE(l.isGameOver());
}
