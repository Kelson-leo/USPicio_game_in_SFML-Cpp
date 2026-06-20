#pragma once

#include "core/Drawable.h"
#include "core/HealthComponent.h"
#include "core/DamageConfig.h"
#include "core/PhysicsConstants.h"
#include "infrastructure/SfmlSprite.h"
#include "infrastructure/FrameConfig.h"
#include <SFML/System/Vector2.hpp>

namespace gameplay {

class Player;

class Professor : public core::Drawable {
public:
    Professor(const sf::Texture& texture,
              infrastructure::FrameConfig& frameConfig);

    void draw(core::IRenderer& renderer) const override;

    /// Shoots a book (BossProjectile) at the player.
    void shootBook(Player& player, const core::DamageConfig& cfg);

    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);

    core::HealthComponent health;

private:
    infrastructure::SfmlSprite   m_sprite;
    infrastructure::FrameConfig& m_frameConfig;
    sf::Vector2f                 m_position;
};

} // namespace gameplay
