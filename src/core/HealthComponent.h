#pragma once

#include <algorithm>

namespace core {

struct HealthComponent {
    int maxHP     = 100;
    int currentHP = 100;

    void takeDamage(int amount, bool isDefending = false) {
        int finalDamage = amount;
        if (isDefending) {
            finalDamage = std::max(1, amount / 5);  // 80% reduction
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
