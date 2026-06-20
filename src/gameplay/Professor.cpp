#include "gameplay/Professor.h"
#include "gameplay/Player.h"

namespace gameplay {

Professor::Professor(const sf::Texture& texture,
                     infrastructure::FrameConfig& frameConfig)
    : m_sprite(texture)
    , m_frameConfig(frameConfig)
    , m_position{600.0f, core::GROUND_Y} {
    health.maxHP     = 80;
    health.currentHP = 80;

    // Set idle texture rect
    auto rect = m_frameConfig.getFrame("professor", "idle", 0);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
    m_sprite.setPosition(m_position.x, m_position.y);
}

void Professor::draw(core::IRenderer& renderer) const {
    m_sprite.draw(renderer);
}

void Professor::shootBook(Player& player, const core::DamageConfig& cfg) {
    player.takeHit(core::AttackType::BossProjectile, cfg);
}

sf::Vector2f Professor::getPosition() const {
    return m_position;
}

void Professor::setPosition(sf::Vector2f pos) {
    m_position = pos;
    m_sprite.setPosition(pos.x, pos.y);
}

} // namespace gameplay
