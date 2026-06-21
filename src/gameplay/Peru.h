#pragma once

#include "gameplay/Boss.h"

namespace gameplay {

class Peru : public Boss {
public:
    Peru(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig);

protected:
    ProjectileType getProjectileType() const override;
    void performMeleeAttack(Player& player) override;
};

} // namespace gameplay
