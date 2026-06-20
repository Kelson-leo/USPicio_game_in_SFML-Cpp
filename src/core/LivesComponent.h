#pragma once

#include <algorithm>

namespace core {

struct LivesComponent {
    int maxLives     = 5;
    int currentLives = 5;

    void loseLife() {
        currentLives = std::max(0, currentLives - 1);
    }

    bool isGameOver() const {
        return currentLives <= 0;
    }
};

} // namespace core
