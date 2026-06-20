#include "gameplay/Capivara.h"
#include "gameplay/Player.h"

namespace gameplay {

Capivara::Capivara()
    : m_position{400.0f, core::GROUND_Y} {
    health.maxHP     = 30;
    health.currentHP = 30;
}

void Capivara::touchPlayer(Player& player, const core::DamageConfig& cfg) {
    player.takeHit(core::AttackType::EnemyTouch, cfg);
}

sf::Vector2f Capivara::getPosition() const {
    return m_position;
}

void Capivara::setPosition(sf::Vector2f pos) {
    m_position = pos;
}

} // namespace gameplay
