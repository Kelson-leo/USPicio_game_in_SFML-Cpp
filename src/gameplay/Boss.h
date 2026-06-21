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
#include <string>
#include <vector>

namespace gameplay {

class Player;

/// Base class for all boss enemies (Professor, Rato, Mandrake, Peru).
/// Subclasses override getProjectileType() and performMeleeAttack() for unique behavior.
class Boss : public core::Drawable {
public:
    Boss(const sf::Texture& texture,
         infrastructure::FrameConfig& frameConfig,
         const std::string& bossName,
         float startX);

    virtual ~Boss() = default;

    void draw(core::IRenderer& renderer) const override;

    virtual void update(float dt, const Player& player,
                        std::vector<std::unique_ptr<Projectile>>& projectiles,
                        infrastructure::FrameConfig& frameConfig,
                        const sf::Texture& examTexture);

    virtual void shootProjectile(std::vector<std::unique_ptr<Projectile>>& projectiles,
                                 infrastructure::FrameConfig& frameConfig,
                                 const sf::Texture& texture,
                                 const Player& player);

    /// Melee attack — override in subclasses that support it.
    virtual void performMeleeAttack(Player& player);

    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);
    void setGroundY(float y);
    void setCanMelee(bool can);

    core::HealthComponent health;

    static constexpr float RANGED_COOLDOWN = 2.0f;
    static constexpr float RANGED_RANGE    = 600.0f;
    static constexpr float MELEE_RANGE     = 300.0f;
    static constexpr float MELEE_COOLDOWN  = 1.5f;
    static constexpr float BOSS_HEIGHT     = 80.0f;

protected:
    /// Which projectile type this boss shoots (subclasses override).
    virtual ProjectileType getProjectileType() const;

    infrastructure::SfmlSprite   m_sprite;
    infrastructure::FrameConfig& m_frameConfig;
    std::string                  m_bossName;
    sf::Vector2f                 m_position;
    float                        m_groundY           = core::GROUND_Y;
    float                        m_rangedCooldown     = 0.0f;
    float                        m_meleeCooldown      = 0.0f;
    bool                         m_canMelee           = false;
};

} // namespace gameplay
