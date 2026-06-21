#pragma once

#include <algorithm>
#include <cmath>

namespace core {

struct HealthComponent {
    int maxHP     = 100;
    int currentHP = 100;

    void takeDamage(int amount, bool isDefending = false) {
        int finalDamage = amount;
        if (isDefending) {
            // 70% reduction → player takes 30% of damage, minimum 1
            finalDamage = std::max(1, static_cast<int>(std::ceil(amount * 0.3f)));
        }
        currentHP = std::max(0, currentHP - finalDamage);
    }

    void heal(int amount) {
        currentHP = std::min(maxHP, currentHP + amount);
    }

    bool isDead() const {
        return currentHP <= 0;
    }
};

} // namespace core
