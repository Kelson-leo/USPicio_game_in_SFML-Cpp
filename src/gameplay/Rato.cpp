#include "gameplay/Rato.h"
#include "gameplay/Player.h"

namespace gameplay {

Rato::Rato(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig)
    : Boss(texture, frameConfig, "rato", 600.0f) {
    // Apply scale for the large native frames (345×390 → ~86×97)
    m_sprite.setScale(RATO_SCALE, RATO_SCALE);
    // Start looping attack animation
    m_currentAnim = "attack_right";
    syncSpriteRect();
}

void Rato::draw(core::IRenderer& renderer) const {
    m_sprite.draw(renderer);
}

void Rato::update(float dt, const Player& player,
                  std::vector<std::unique_ptr<Projectile>>& projectiles,
                  infrastructure::FrameConfig& frameConfig) {
    if (health.isDead()) return;
    updateAnimation(dt);
    Boss::update(dt, player, projectiles, frameConfig);
}

void Rato::updateAnimation(float dt) {
    m_frameTimer += dt;
    if (m_frameTimer >= FRAME_DURATION) {
        m_frameTimer = 0.0f;
        auto count = m_frameConfig.frameCount("rato", m_currentAnim);
        if (count > 0) {
            // Continuous loop through all 6 frames
            m_frameIndex = (m_frameIndex + 1) % count;
            syncSpriteRect();
        }
    }
}

void Rato::syncSpriteRect() {
    auto rect = m_frameConfig.getFrame("rato", m_currentAnim, m_frameIndex);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
}

ProjectileType Rato::getProjectileType() const {
    return ProjectileType::Pan;
}

} // namespace gameplay
