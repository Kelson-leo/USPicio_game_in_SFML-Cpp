#include "gameplay/Professor.h"
#include "gameplay/Player.h"

namespace gameplay {

Professor::Professor(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig)
    : Boss(texture, frameConfig, "professor", 600.0f) {
    m_sprite.setScale(PROFESSOR_SCALE, PROFESSOR_SCALE);

    m_useIdleV2 = (std::rand() % 100) < static_cast<int>(IDLE_V2_CHANCE * 100);
    {
        auto idleAnim = m_useIdleV2 ? "idle_right_v2" : "idle_right";
        setAnim(idleAnim, IDLE_FRAME_DURATION, true);
    }
}

void Professor::draw(core::IRenderer& renderer) const {
    m_sprite.draw(renderer);
}

void Professor::update(float dt, const Player& player,
                       std::vector<std::unique_ptr<Projectile>>& projectiles,
                       infrastructure::FrameConfig& frameConfig) {
    if (health.isDead()) return;

    // Handle hurt timer — when it expires, return to idle
    if (m_animState == AnimState::Hurt) {
        m_hurtTimer -= dt;
        if (m_hurtTimer <= 0.0f) {
            returnToIdle();
        }
        updateAnimation(dt);
        return;  // no attacks while hurt
    }

    updateAnimation(dt);
    Boss::update(dt, player, projectiles, frameConfig);
}

void Professor::shootProjectile(
    std::vector<std::unique_ptr<Projectile>>& projectiles,
    infrastructure::FrameConfig& frameConfig,
    const Player& player) {
    m_animState = AnimState::RangedAttack;
    setAnim("attack_ranged_right", RANGED_FRAME_DURATION, false);
    Boss::shootProjectile(projectiles, frameConfig, player);
}

void Professor::onHit() {
    m_animState = AnimState::Hurt;
    m_hurtTimer = HURT_DURATION;
    setAnim("hurt_right", HURT_DURATION, false);
}

void Professor::updateAnimation(float dt) {
    m_frameTimer += dt;
    if (m_frameTimer >= m_frameDuration) {
        m_frameTimer = 0.0f;
        auto count = m_frameConfig.frameCount("professor", m_currentAnim);
        if (count > 0) {
            m_frameIndex++;
            if (m_frameIndex >= count) {
                if (m_loop) {
                    m_frameIndex = 0;
                } else {
                    // Attack finished — re-roll idle and return
                    if (m_animState == AnimState::RangedAttack) {
                        m_useIdleV2 = (std::rand() % 100)
                            < static_cast<int>(IDLE_V2_CHANCE * 100);
                    }
                    returnToIdle();
                    return;
                }
            }
            syncSpriteRect();
        }
    }
}

void Professor::setAnim(const std::string& anim, float duration, bool loop) {
    m_currentAnim   = anim;
    m_frameDuration = duration;
    m_loop          = loop;
    m_frameIndex    = 0;
    m_frameTimer    = 0.0f;
    syncSpriteRect();
}

void Professor::returnToIdle() {
    m_animState = AnimState::Idle;
    auto idleAnim = m_useIdleV2 ? "idle_right_v2" : "idle_right";
    setAnim(idleAnim, IDLE_FRAME_DURATION, true);
}

void Professor::syncSpriteRect() {
    auto rect = m_frameConfig.getFrame("professor", m_currentAnim, m_frameIndex);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
}

ProjectileType Professor::getProjectileType() const {
    return ProjectileType::Exam;
}

} // namespace gameplay
