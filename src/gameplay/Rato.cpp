#include "gameplay/Rato.h"

namespace gameplay {

Rato::Rato(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig)
    : Boss(texture, frameConfig, "rato", 600.0f) {
}

ProjectileType Rato::getProjectileType() const {
    return ProjectileType::Pan;
}

} // namespace gameplay
