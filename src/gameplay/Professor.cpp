#include "gameplay/Professor.h"
#include "gameplay/Player.h"

namespace gameplay {

Professor::Professor()
    : m_position{600.0f, core::GROUND_Y} {
    health.maxHP     = 80;
    health.currentHP = 80;
}

void Professor::shootBook(Player& player, const core::DamageConfig& cfg) {
    player.takeHit(core::AttackType::BossProjectile, cfg);
}

sf::Vector2f Professor::getPosition() const {
    return m_position;
}

void Professor::setPosition(sf::Vector2f pos) {
    m_position = pos;
}

} // namespace gameplay
