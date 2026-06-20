#include "gameplay/Player.h"

namespace gameplay {

Player::Player()
    : m_position{100.0f, core::GROUND_Y} {
    // Default health/lives/ammo from components
}

void Player::punch(core::HealthComponent& enemyHealth,
                   core::EntityType enemyType,
                   const core::DamageConfig& cfg) {
    int dmg = cfg.getDamage(core::AttackType::Punch, enemyType);
    enemyHealth.takeDamage(dmg);
}

void Player::throwCaneta(core::HealthComponent& enemyHealth,
                         core::EntityType enemyType,
                         const core::DamageConfig& cfg) {
    if (!ammo.canUse()) return;

    ammo.use();
    int dmg = cfg.getDamage(core::AttackType::Throw, enemyType);
    enemyHealth.takeDamage(dmg);
}

void Player::defend(bool active) {
    m_isDefending = active;
}

bool Player::isDefending() const {
    return m_isDefending;
}

void Player::takeHit(core::AttackType attack,
                     const core::DamageConfig& cfg) {
    int dmg = cfg.getDamage(attack, core::EntityType::Player);
    health.takeDamage(dmg, m_isDefending);

    if (health.isDead()) {
        lives.loseLife();
    }
}

bool Player::revive() {
    if (lives.isGameOver()) return false;

    health.currentHP = health.maxHP;
    m_position = {100.0f, core::GROUND_Y};
    return true;
}

sf::Vector2f Player::getPosition() const {
    return m_position;
}

void Player::setPosition(sf::Vector2f pos) {
    m_position = pos;
}

} // namespace gameplay
