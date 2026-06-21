#pragma once

#include "gameplay/Boss.h"
#include <cstdlib>

namespace gameplay {

class Mandrake : public Boss {
public:
    Mandrake(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig);

    void draw(core::IRenderer& renderer) const override;
    void update(float dt, const Player& player,
                std::vector<std::unique_ptr<Projectile>>& projectiles,
                infrastructure::FrameConfig& frameConfig) override;
    void shootProjectile(std::vector<std::unique_ptr<Projectile>>& projectiles,
                         infrastructure::FrameConfig& frameConfig,
                         const Player& player) override;
    void performMeleeAttack(Player& player) override;

    float getHeight() const override { return FRAME_H * MANDRAKE_SCALE; }  // ~95.4

    static constexpr float MANDRAKE_SCALE        = 0.3f;
    static constexpr float FRAME_H               = 318.0f;
    static constexpr float IDLE_V2_CHANCE        = 0.5f;
    static constexpr float RANGED_FRAME_DURATION = 0.12f;
    static constexpr float MELEE_FRAME_DURATION  = 0.10f;

protected:
    ProjectileType getProjectileType() const override;

private:
    enum class AnimState { Idle, RangedAttack, MeleeAttack };

    void updateAnimation(float dt);
    void setAnim(const std::string& anim, float duration, bool loop);
    void syncSpriteRect();

    AnimState   m_animState = AnimState::Idle;
    std::string m_currentAnim;
    std::size_t m_frameIndex = 0;
    float       m_frameTimer = 0.0f;
    float       m_frameDuration = RANGED_FRAME_DURATION;
    bool        m_loop       = false;
    bool        m_useIdleV2  = false;
};

} // namespace gameplay
