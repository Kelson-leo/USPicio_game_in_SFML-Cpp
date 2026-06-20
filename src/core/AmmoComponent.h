#pragma once

#include <algorithm>

namespace core {

struct AmmoComponent {
    int maxAmmo     = 10;
    int currentAmmo = 10;

    bool canUse() const {
        return currentAmmo > 0;
    }

    void use() {
        if (currentAmmo > 0) {
            --currentAmmo;
        }
    }

    void reload(int amount) {
        currentAmmo = std::min(maxAmmo, currentAmmo + amount);
    }
};

} // namespace core
