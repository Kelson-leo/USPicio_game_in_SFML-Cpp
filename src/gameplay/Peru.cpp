#include "gameplay/Peru.h"
#include "gameplay/Player.h"

namespace gameplay {

Peru::Peru(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig)
    : Boss(texture, frameConfig, "peru", 600.0f) {
    m_canMelee = true;
}

ProjectileType Peru::getProjectileType() const {
    return ProjectileType::Cup;
}

void Peru::performMeleeAttack(Player& player) {
    // Bicada: fast but lighter damage
    player.health.takeDamage(6);
    m_meleeCooldown = MELEE_COOLDOWN * 0.8f;  // 1.2s
}

} // namespace gameplay
