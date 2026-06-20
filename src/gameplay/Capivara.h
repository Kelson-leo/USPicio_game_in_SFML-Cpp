#pragma once

#include "core/Drawable.h"
#include "core/Direction.h"
#include "core/HealthComponent.h"
#include "core/DamageConfig.h"
#include "core/PhysicsConstants.h"
#include "infrastructure/SfmlSprite.h"
#include "infrastructure/FrameConfig.h"
#include <SFML/System/Vector2.hpp>
#include <string>

namespace gameplay {

class Player;

class Capivara : public core::Drawable {
public:
    static constexpr float SPEED           = 40.0f;
    static constexpr float FRAME_DURATION  = 0.18f;
    static constexpr float SCALE           = 1.5f;
    static constexpr float FRAME_HEIGHT    = 55.0f * SCALE;   // 82.5
    static constexpr float ATTACK_RANGE    = 30.0f;
    static constexpr float ATTACK_COOLDOWN = 1.0f;
    static constexpr float SCREEN_MIN_X    = 0.0f;
    static constexpr float SCREEN_MAX_X    = 1860.0f;

    Capivara(const sf::Texture& texture,
             infrastructure::FrameConfig& frameConfig,
             float startX);

    void draw(core::IRenderer& renderer) const override;

    void update(float dt, const Player& player);
    void takeDamage(int amount);
    void touchPlayer(Player& player, const core::DamageConfig& cfg);

    bool isDead() const;
    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);

    core::HealthComponent health;

private:
    std::string buildAnimName(const std::string& action) const;
    void setAnimation(const std::string& action);
    void applyGravity(float dt);

    infrastructure::SfmlSprite   m_sprite;
    infrastructure::FrameConfig& m_frameConfig;
    core::Direction              m_direction = core::Direction::Left;
    std::string                  m_currentAnim;
    std::size_t                  m_frameIndex = 0;
    float                        m_frameTimer = 0.0f;
    sf::Vector2f                 m_position;
    float                        m_velocityY = 0.0f;
    float                        m_attackCooldown = 0.0f;
    bool                         m_isAlive = true;
};

} // namespace gameplay
