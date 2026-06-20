#pragma once

#include "core/HealthComponent.h"
#include "core/LivesComponent.h"
#include "core/AmmoComponent.h"
#include "core/DamageConfig.h"
#include "core/PhysicsConstants.h"
#include <SFML/System/Vector2.hpp>

namespace gameplay {

class Player {
public:
    Player();

    // ── Offense ─────────────────────────────────────────────────────
    void punch(core::HealthComponent& enemyHealth,
               core::EntityType enemyType,
               const core::DamageConfig& cfg);

    void throwCaneta(core::HealthComponent& enemyHealth,
                     core::EntityType enemyType,
                     const core::DamageConfig& cfg);

    // ── Defense ─────────────────────────────────────────────────────
    void defend(bool active);
    bool isDefending() const;

    // ── Damage to self ──────────────────────────────────────────────
    void takeHit(core::AttackType attack,
                 const core::DamageConfig& cfg);

    /// Call after HP reaches 0: lose a life & reset HP.
    /// Returns true if the player revived successfully.
    bool revive();

    // ── Position ────────────────────────────────────────────────────
    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);

    // ── Components (public for easy access by UI) ───────────────────
    core::HealthComponent health;
    core::LivesComponent   lives;
    core::AmmoComponent    ammo;

private:
    sf::Vector2f m_position;
    bool         m_isDefending = false;
};

} // namespace gameplay
