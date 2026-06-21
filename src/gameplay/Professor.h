#pragma once

#include "core/Drawable.h"
#include "core/HealthComponent.h"
#include "core/DamageConfig.h"
#include "core/PhysicsConstants.h"
#include "infrastructure/SfmlSprite.h"
#include "infrastructure/FrameConfig.h"
#include "gameplay/Projectile.h"
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>

namespace gameplay {

class Player;

class Professor : public core::Drawable {
public:
    Professor(const sf::Texture& texture,
              infrastructure::FrameConfig& frameConfig);

    void draw(core::IRenderer& renderer) const override;

    void update(float dt, const Player& player,
                std::vector<std::unique_ptr<Projectile>>& projectiles,
                infrastructure::FrameConfig& frameConfig,
                const sf::Texture& examTexture);

    void shootBook(Player& player, const core::DamageConfig& cfg);

    /// Shoots an exam projectile toward the player.
    void shootProjectile(std::vector<std::unique_ptr<Projectile>>& projectiles,
                         infrastructure::FrameConfig& frameConfig,
                         const sf::Texture& texture,
                         const Player& player);

    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);
    void setGroundY(float y);

    core::HealthComponent health;

    static constexpr float ATTACK_COOLDOWN = 2.0f;
    static constexpr float ATTACK_RANGE    = 600.0f;
    static constexpr float PROFESSOR_HEIGHT = 80.0f;

private:
    infrastructure::SfmlSprite   m_sprite;
    infrastructure::FrameConfig& m_frameConfig;
    sf::Vector2f                 m_position;
    float                        m_groundY = core::GROUND_Y;
    float                        m_attackCooldown = 0.0f;
};

} // namespace gameplay
