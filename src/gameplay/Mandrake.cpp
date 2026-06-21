#include "gameplay/Mandrake.h"
#include "gameplay/Player.h"

namespace gameplay {

Mandrake::Mandrake(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig)
    : Boss(texture, frameConfig, "mandrake", 600.0f) {
    m_canMelee = true;
}

ProjectileType Mandrake::getProjectileType() const {
    return ProjectileType::Stone;
}

void Mandrake::performMeleeAttack(Player& player) {
    player.health.takeDamage(8);
}

} // namespace gameplay
