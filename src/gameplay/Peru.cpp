#include "gameplay/Peru.h"
#include "gameplay/Player.h"

namespace gameplay {

Peru::Peru(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig)
    : Boss(texture, frameConfig, "peru", 600.0f) {
    m_canMelee = true;
    m_sprite.setScale(PERU_SCALE, PERU_SCALE);

    m_useIdleV2 = (std::rand() % 100) < static_cast<int>(IDLE_V2_CHANCE * 100);
    {
        auto idleAnim = m_useIdleV2 ? "idle_right_v2" : "idle_right";
        setAnim(idleAnim, IDLE_FRAME_DURATION, true);
    }
}

void Peru::draw(core::IRenderer& renderer) const {
    m_sprite.draw(renderer);
}

void Peru::update(float dt, const Player& player,
                  std::vector<std::unique_ptr<Projectile>>& projectiles,
                  infrastructure::FrameConfig& frameConfig) {
    if (health.isDead()) return;
    updateAnimation(dt);
    Boss::update(dt, player, projectiles, frameConfig);
}

void Peru::shootProjectile(
    std::vector<std::unique_ptr<Projectile>>& projectiles,
    infrastructure::FrameConfig& frameConfig,
    const Player& player) {
    m_animState = AnimState::RangedAttack;
    setAnim("attack_ranged_right", RANGED_FRAME_DURATION, false);
    Boss::shootProjectile(projectiles, frameConfig, player);
}

void Peru::performMeleeAttack(Player& player) {
    m_animState = AnimState::MeleeAttack;
    setAnim("attack_melee_right", MELEE_FRAME_DURATION, false);
    player.health.takeDamage(6);
}

void Peru::updateAnimation(float dt) {
    m_frameTimer += dt;
    if (m_frameTimer >= m_frameDuration) {
        m_frameTimer = 0.0f;
        auto count = m_frameConfig.frameCount("peru", m_currentAnim);
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

void Peru::setAnim(const std::string& anim, float duration, bool loop) {
    m_currentAnim   = anim;
    m_frameDuration = duration;
    m_loop          = loop;
    m_frameIndex    = 0;
    m_frameTimer    = 0.0f;
    syncSpriteRect();
}

void Peru::syncSpriteRect() {
    auto rect = m_frameConfig.getFrame("peru", m_currentAnim, m_frameIndex);
    m_sprite.setTextureRect(sf::Rect2i({rect.position.x, rect.position.y},
                                        {rect.size.x, rect.size.y}));
}

ProjectileType Peru::getProjectileType() const {
    return ProjectileType::Cup;
}

} // namespace gameplay
