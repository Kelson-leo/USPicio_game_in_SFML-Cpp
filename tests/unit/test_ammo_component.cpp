#include <gtest/gtest.h>
#include "core/AmmoComponent.h"

TEST(AmmoComponentTest, InitialValues) {
    core::AmmoComponent a;
    EXPECT_EQ(a.maxAmmo, 10);
    EXPECT_EQ(a.currentAmmo, 10);
    EXPECT_TRUE(a.canUse());
}

TEST(AmmoComponentTest, UseDecrementsAmmo) {
    core::AmmoComponent a;
    a.use();
    EXPECT_EQ(a.currentAmmo, 9);
    EXPECT_TRUE(a.canUse());
}

TEST(AmmoComponentTest, CanUseFalseWhenEmpty) {
    core::AmmoComponent a;
    for (int i = 0; i < 10; ++i) {
        a.use();
    }
    EXPECT_EQ(a.currentAmmo, 0);
    EXPECT_FALSE(a.canUse());
}

TEST(AmmoComponentTest, UseWhenEmptyDoesNothing) {
    core::AmmoComponent a;
    for (int i = 0; i < 10; ++i) {
        a.use();
    }
    a.use();  // 11th use — should be no-op
    EXPECT_EQ(a.currentAmmo, 0);
}

TEST(AmmoComponentTest, ReloadRestoresAmmo) {
    core::AmmoComponent a;
    for (int i = 0; i < 10; ++i) {
        a.use();
    }
    a.reload(5);
    EXPECT_EQ(a.currentAmmo, 5);
    EXPECT_TRUE(a.canUse());
}

TEST(AmmoComponentTest, ReloadDoesNotExceedMax) {
    core::AmmoComponent a;
    a.use();
    a.use();
    EXPECT_EQ(a.currentAmmo, 8);
    a.reload(10);  // try to add 10, should cap at maxAmmo
    EXPECT_EQ(a.currentAmmo, a.maxAmmo);
}
