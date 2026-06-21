#pragma once

#include "core/Drawable.h"
#include "core/Direction.h"
#include "core/HealthComponent.h"
#include "core/LivesComponent.h"
#include "core/AmmoComponent.h"
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

class Player : public core::Drawable {
public:
    Player(const sf::Texture& texture, infrastructure::FrameConfig& frameConfig);

    // ── Animation ──────────────────────────────────────────────────
    void setAnimation(const std::string& action);  // e.g. "idle", "walk", "punch"
    void updateAnimation(float dt);
    bool isAttacking() const;  // true while punch/throw animation is playing

    static constexpr float FRAME_DURATION = 0.15f;
    static constexpr float WALK_SPEED     = 80.0f;
    static constexpr float PLAYER_SCALE   = 1.5f;
    static constexpr float PLAYER_HEIGHT  = 95.0f * PLAYER_SCALE;  // feet offset
    static constexpr float CROUCH_HEIGHT  = 79.0f * PLAYER_SCALE;  // crouch hitbox
    static constexpr float SHOOT_COOLDOWN = 0.3f;

    // ── core::Drawable ─────────────────────────────────────────────
    void draw(core::IRenderer& renderer) const override;

    // ── Direction ──────────────────────────────────────────────────
    core::Direction getDirection() const;
    void setDirection(core::Direction dir);

    // ── Offense ─────────────────────────────────────────────────────
    void punch(core::HealthComponent& enemyHealth,
               core::EntityType enemyType,
               const core::DamageConfig& cfg);

    void throwCaneta(core::HealthComponent& enemyHealth,
                     core::EntityType enemyType,
                     const core::DamageConfig& cfg);

    void throwProjectile(std::vector<std::unique_ptr<Projectile>>& projectiles,
                         infrastructure::FrameConfig& frameConfig,
                         const sf::Texture& texture);

    // ── Defense ─────────────────────────────────────────────────────
    void defend(bool active);
    bool isDefending() const;

    // ── Crouch ──────────────────────────────────────────────────────
    void setCrouching(bool crouching);
    bool isCrouching() const;
    float getCurrentHeight() const;

    // ── Damage to self ──────────────────────────────────────────────
    void takeHit(core::AttackType attack,
                 const core::DamageConfig& cfg);

    /// Call after HP reaches 0: lose a life & reset HP.
    /// Returns true if the player revived successfully.
    bool revive();

    // ── Movement ────────────────────────────────────────────────────
    void moveLeft(float dt);
    void moveRight(float dt);
    void applyGravity(float dt);
    bool isOnGround() const;

    // ── Position ────────────────────────────────────────────────────
    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);

    // ── Components (public for easy access by UI) ───────────────────
    core::HealthComponent health;
    core::LivesComponent   lives;
    core::AmmoComponent    ammo;

    // ── Physics ─────────────────────────────────────────────────────
    float velocityY = 0.0f;
    float m_shootCooldown = 0.0f;

private:
    std::string buildAnimName(const std::string& action) const;

    infrastructure::SfmlSprite       m_sprite;
    infrastructure::FrameConfig&     m_frameConfig;
    core::Direction                  m_direction = core::Direction::Right;
    std::string                      m_currentAnim;
    std::size_t                      m_frameIndex = 0;
    float                            m_frameTimer = 0.0f;
    sf::Vector2f                     m_position;
    bool                             m_isDefending  = false;
    bool                             m_isCrouching  = false;
};

} // namespace gameplay
