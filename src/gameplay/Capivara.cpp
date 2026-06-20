#include "gameplay/Capivara.h"
#include "gameplay/Player.h"

namespace gameplay {

Capivara::Capivara(const sf::Texture& texture,
                   infrastructure::FrameConfig& frameConfig)
    : m_sprite(texture)
    , m_frameConfig(frameConfig)
    , m_position{400.0f, core::GROUND_Y} {
    health.maxHP     = 30;
    health.currentHP = 30;

    // Set idle texture rect
    auto rect = m_frameConfig.getFrame("capivara", "idle", 0);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
    m_sprite.setPosition(m_position.x, m_position.y);
}

void Capivara::draw(core::IRenderer& renderer) const {
    m_sprite.draw(renderer);
}

void Capivara::touchPlayer(Player& player, const core::DamageConfig& cfg) {
    player.takeHit(core::AttackType::EnemyTouch, cfg);
}

sf::Vector2f Capivara::getPosition() const {
    return m_position;
}

void Capivara::setPosition(sf::Vector2f pos) {
    m_position = pos;
    m_sprite.setPosition(pos.x, pos.y);
}

} // namespace gameplay
