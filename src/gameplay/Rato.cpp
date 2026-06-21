#include "gameplay/Rato.h"
#include "gameplay/Player.h"

namespace gameplay {

Rato::Rato(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig)
    : Boss(texture, frameConfig, "rato", 600.0f) {
    m_sprite.setScale(RATO_SCALE, RATO_SCALE);
    setAnim("idle_right");
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

void Rato::shootProjectile(
    std::vector<std::unique_ptr<Projectile>>& projectiles,
    infrastructure::FrameConfig& frameConfig,
    const Player& player) {
    // Trigger attack animation (plays through once, then returns to idle)
    m_isAttacking = true;
    setAnim("attack_right");
    Boss::shootProjectile(projectiles, frameConfig, player);
}

void Rato::updateAnimation(float dt) {
    if (!m_isAttacking) return;  // idle: stay on frame 0

    m_frameTimer += dt;
    if (m_frameTimer >= FRAME_DURATION) {
        m_frameTimer = 0.0f;
        auto count = m_frameConfig.frameCount("rato", m_currentAnim);
        if (count > 0) {
            m_frameIndex++;
            if (m_frameIndex >= count) {
                // Attack finished — return to idle
                m_isAttacking = false;
                setAnim("idle_right");
                return;
            }
            syncSpriteRect();
        }
    }
}

void Rato::syncSpriteRect() {
    auto rect = m_frameConfig.getFrame("rato", m_currentAnim, m_frameIndex);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
}

void Rato::setAnim(const std::string& anim) {
    m_currentAnim = anim;
    m_frameIndex  = 0;
    m_frameTimer  = 0.0f;
    syncSpriteRect();
}

ProjectileType Rato::getProjectileType() const {
    return ProjectileType::Pan;
}

} // namespace gameplay
