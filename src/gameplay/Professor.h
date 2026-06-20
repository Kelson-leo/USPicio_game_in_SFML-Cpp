#pragma once

#include "core/HealthComponent.h"
#include "core/DamageConfig.h"
#include <SFML/System/Vector2.hpp>

namespace gameplay {

class Player;

class Professor {
public:
    Professor();

    /// Shoots a book (BossProjectile) at the player.
    void shootBook(Player& player, const core::DamageConfig& cfg);

    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);

    core::HealthComponent health;

private:
    sf::Vector2f m_position;
};

} // namespace gameplay
