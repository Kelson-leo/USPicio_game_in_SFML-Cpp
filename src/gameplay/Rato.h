#pragma once

#include "gameplay/Boss.h"

namespace gameplay {

class Rato : public Boss {
public:
    Rato(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig);

    void draw(core::IRenderer& renderer) const override;
    void update(float dt, const Player& player,
                std::vector<std::unique_ptr<Projectile>>& projectiles,
                infrastructure::FrameConfig& frameConfig) override;

    float getHeight() const override { return RATO_HEIGHT; }

    static constexpr float RATO_SCALE  = 0.25f;
    static constexpr float FRAME_W     = 345.0f;
    static constexpr float FRAME_H     = 390.0f;
    static constexpr float RATO_HEIGHT = FRAME_H * RATO_SCALE;  // ~97.5

protected:
    ProjectileType getProjectileType() const override;

private:
    void updateAnimation(float dt);
    void syncSpriteRect();

    std::string m_currentAnim;
    std::size_t m_frameIndex = 0;
    float       m_frameTimer = 0.0f;
    static constexpr float FRAME_DURATION = 0.12f;
};

} // namespace gameplay
