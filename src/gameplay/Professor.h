#pragma once

#include "gameplay/Boss.h"

namespace gameplay {

class Professor : public Boss {
public:
    Professor(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig)
        : Boss(texture, frameConfig, "professor", 600.0f) {}

    /// Boss projectile vs player (calls Player::takeHit).
    void shootBook(Player& player, const core::DamageConfig& cfg);
};

} // namespace gameplay
