#pragma once

#include "gameplay/Boss.h"
#include <cstdlib>

namespace gameplay {

class Professor : public Boss {
public:
    Professor(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig);

    void draw(core::IRenderer& renderer) const override;
    void update(float dt, const Player& player,
                std::vector<std::unique_ptr<Projectile>>& projectiles,
                infrastructure::FrameConfig& frameConfig) override;
    void shootProjectile(std::vector<std::unique_ptr<Projectile>>& projectiles,
                         infrastructure::FrameConfig& frameConfig,
                         const Player& player) override;
    void onHit() override;

    float getHeight() const override { return MAX_FRAME_H * PROFESSOR_SCALE; }  // ~121
    float getProjectileOffsetY() const override { return -30.0f; }

    static constexpr float PROFESSOR_SCALE       = 0.35f;
    static constexpr float MAX_FRAME_H           = 345.0f;  // hurt frame
    static constexpr float IDLE_V2_CHANCE        = 0.5f;
    static constexpr float RANGED_FRAME_DURATION = 0.12f;
    static constexpr float HURT_DURATION         = 0.3f;

protected:
    ProjectileType getProjectileType() const override;

private:
    enum class AnimState { Idle, RangedAttack, Hurt };

    void updateAnimation(float dt);
    void setAnim(const std::string& anim, float duration, bool loop);
    void syncSpriteRect();
    void returnToIdle();

    AnimState   m_animState = AnimState::Idle;
    std::string m_currentAnim;
    std::size_t m_frameIndex = 0;
    float       m_frameTimer = 0.0f;
    float       m_frameDuration = RANGED_FRAME_DURATION;
    float       m_hurtTimer      = 0.0f;
    bool        m_loop       = false;
    bool        m_useIdleV2  = false;
    static constexpr float IDLE_FRAME_DURATION = 0.3f;
};

} // namespace gameplay
