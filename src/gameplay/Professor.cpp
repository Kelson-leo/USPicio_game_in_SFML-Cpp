#include "gameplay/Professor.h"
#include "gameplay/Player.h"
#include <cmath>

namespace gameplay {

Professor::Professor(const sf::Texture& texture,
                     infrastructure::FrameConfig& frameConfig)
    : m_sprite(texture)
    , m_frameConfig(frameConfig)
    , m_position{600.0f, core::GROUND_Y} {
    health.maxHP     = 80;
    health.currentHP = 80;

    auto rect = m_frameConfig.getFrame("professor", "idle", 0);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
    m_sprite.setPosition(m_position.x, m_position.y);
}

void Professor::draw(core::IRenderer& renderer) const {
    m_sprite.draw(renderer);
}

void Professor::update(float dt, const Player& player,
                       std::vector<std::unique_ptr<Projectile>>& projectiles,
                       infrastructure::FrameConfig& frameConfig,
                       const sf::Texture& examTexture) {
    if (health.isDead()) return;

    m_attackCooldown -= dt;

    float dx = std::abs(player.getPosition().x - m_position.x);
    if (m_attackCooldown <= 0.0f && dx < ATTACK_RANGE) {
        shootProjectile(projectiles, frameConfig, examTexture, player);
    }
}

void Professor::shootBook(Player& player, const core::DamageConfig& cfg) {
    player.takeHit(core::AttackType::BossProjectile, cfg);
}

void Professor::shootProjectile(
    std::vector<std::unique_ptr<Projectile>>& projectiles,
    infrastructure::FrameConfig& frameConfig,
    const sf::Texture& texture,
    const Player& player) {

    if (health.isDead()) return;

    auto pPos = player.getPosition();
    core::Direction dir = (pPos.x > m_position.x)
        ? core::Direction::Right
        : core::Direction::Left;

    auto proj = std::make_unique<Projectile>();
    sf::Vector2f offset = (dir == core::Direction::Right)
        ? sf::Vector2f(30.0f, 20.0f)
        : sf::Vector2f(-60.0f, 20.0f);
    proj->init(ProjectileType::Exam, dir, texture,
               frameConfig, m_position + offset);
    projectiles.push_back(std::move(proj));

    m_attackCooldown = ATTACK_COOLDOWN;
}

sf::Vector2f Professor::getPosition() const {
    return m_position;
}

void Professor::setPosition(sf::Vector2f pos) {
    m_position = pos;
    m_sprite.setPosition(pos.x, pos.y);
}

} // namespace gameplay
