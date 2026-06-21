#pragma once

#include "gameplay/Boss.h"

namespace gameplay {

class Rato : public Boss {
public:
    Rato(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig);

protected:
    ProjectileType getProjectileType() const override;
};

} // namespace gameplay
