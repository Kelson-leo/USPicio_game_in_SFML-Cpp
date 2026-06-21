#pragma once

#include "gameplay/Boss.h"

namespace gameplay {

class Mandrake : public Boss {
public:
    Mandrake(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig);

protected:
    ProjectileType getProjectileType() const override;
    void performMeleeAttack(Player& player) override;
};

} // namespace gameplay
