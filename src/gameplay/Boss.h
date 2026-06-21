#pragma once

#include "core/Drawable.h"
#include "core/HealthComponent.h"
#include "core/DamageConfig.h"
#include "core/PhysicsConstants.h"
#include "infrastructure/SfmlSprite.h"
#include "infrastructure/FrameConfig.h"
#include "gameplay/Projectile.h"
#include <SFML/System/Vec2.hpp>
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
                        infrastructure::FrameConfig& frameConfig);

    virtual void shootProjectile(std::vector<std::unique_ptr<Projectile>>& projectiles,
                                 infrastructure::FrameConfig& frameConfig,
                                 const Player& player);

    /// Melee attack — override in subclasses that support it.
    virtual void performMeleeAttack(Player& player);

    /// Called when the boss takes damage. Subclasses can override for hurt animation.
    virtual void onHit() {}

    sf::Vec2f getPosition() const;
    void setPosition(sf::Vec2f pos);
    void setGroundY(float y);
    void setCanMelee(bool can);
    void setProjectileTexture(const sf::Texture& tex);

    /// Boss sprite height for ground alignment (subclasses can override).
    virtual float getHeight() const { return BOSS_HEIGHT; }

    /// Y offset for projectile spawn relative to boss position.
    /// Subclasses override to adjust projectile height (e.g. Mandrake stone).
    virtual float getProjectileOffsetY() const { return -10.0f; }

    core::HealthComponent health;

    static constexpr float RANGED_COOLDOWN = 2.0f;
    static constexpr float RANGED_RANGE    = 600.0f;
    static constexpr float MELEE_RANGE     = 150.0f;
    static constexpr float MELEE_COOLDOWN  = 1.5f;
    static constexpr float BOSS_HEIGHT     = 80.0f;

protected:
    /// Which projectile type this boss shoots (subclasses override).
    virtual ProjectileType getProjectileType() const;

    infrastructure::SfmlSprite   m_sprite;
    infrastructure::FrameConfig& m_frameConfig;
    std::string                  m_bossName;
    const sf::Texture*           m_projectileTexture = nullptr;
    sf::Vec2f                 m_position;
    float                        m_groundY           = core::GROUND_Y;
    float                        m_rangedCooldown     = 0.0f;
    float                        m_meleeCooldown      = 0.0f;
    bool                         m_canMelee           = false;
};

} // namespace gameplay
