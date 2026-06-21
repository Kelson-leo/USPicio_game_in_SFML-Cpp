#include "gameplay/Mandrake.h"
#include "gameplay/Player.h"

namespace gameplay {

Mandrake::Mandrake(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig)
    : Boss(texture, frameConfig, "mandrake", 600.0f) {
    m_canMelee = true;
    m_sprite.setScale(MANDRAKE_SCALE, MANDRAKE_SCALE);

    // 50% chance of idle v2 (2-frame loop) vs idle v1 (single frame)
    m_useIdleV2 = (std::rand() % 100) < static_cast<int>(IDLE_V2_CHANCE * 100);
    {
        auto idleAnim = m_useIdleV2 ? "idle_right_v2" : "idle_right";
        setAnim(idleAnim, IDLE_FRAME_DURATION, true);
    }
}

void Mandrake::draw(core::IRenderer& renderer) const {
    m_sprite.draw(renderer);
}

void Mandrake::update(float dt, const Player& player,
                      std::vector<std::unique_ptr<Projectile>>& projectiles,
                      infrastructure::FrameConfig& frameConfig) {
    if (health.isDead()) return;
    updateAnimation(dt);
    Boss::update(dt, player, projectiles, frameConfig);
}

void Mandrake::shootProjectile(
    std::vector<std::unique_ptr<Projectile>>& projectiles,
    infrastructure::FrameConfig& frameConfig,
    const Player& player) {
    m_animState = AnimState::RangedAttack;
    setAnim("attack_ranged_right", RANGED_FRAME_DURATION, false);
    Boss::shootProjectile(projectiles, frameConfig, player);
}

void Mandrake::performMeleeAttack(Player& player) {
    m_animState = AnimState::MeleeAttack;
    setAnim("attack_melee_right", MELEE_FRAME_DURATION, false);
    player.health.takeDamage(8);
}

void Mandrake::updateAnimation(float dt) {
    m_frameTimer += dt;
    if (m_frameTimer >= m_frameDuration) {
        m_frameTimer = 0.0f;
        auto count = m_frameConfig.frameCount("mandrake", m_currentAnim);
        if (count > 0) {
            m_frameIndex++;
            if (m_frameIndex >= count) {
                if (m_loop) {
                    m_frameIndex = 0;  // loop back
                } else {
                    // Attack finished — return to idle
                    // Re-roll idle version after ranged attack, keep after melee
                    if (m_animState == AnimState::RangedAttack) {
                        m_useIdleV2 = (std::rand() % 100)
                            < static_cast<int>(IDLE_V2_CHANCE * 100);
                    }
                    m_animState = AnimState::Idle;
                    {
                        auto idleAnim = m_useIdleV2 ? "idle_right_v2" : "idle_right";
                        setAnim(idleAnim, IDLE_FRAME_DURATION, true);
                    }
                    return;
                }
            }
            syncSpriteRect();
        }
    }
}

void Mandrake::setAnim(const std::string& anim, float duration, bool loop) {
    m_currentAnim   = anim;
    m_frameDuration = duration;
    m_loop          = loop;
    m_frameIndex    = 0;
    m_frameTimer    = 0.0f;
    syncSpriteRect();
}

void Mandrake::syncSpriteRect() {
    auto rect = m_frameConfig.getFrame("mandrake", m_currentAnim, m_frameIndex);
    m_sprite.setTextureRect(sf::Rect2i({rect.position.x, rect.position.y},
                                        {rect.size.x, rect.size.y}));
}

ProjectileType Mandrake::getProjectileType() const {
    return ProjectileType::Stone;
}

} // namespace gameplay
