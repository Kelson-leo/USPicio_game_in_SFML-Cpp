#pragma once

#include "core/Drawable.h"
#include "core/Direction.h"
#include "infrastructure/SfmlSprite.h"
#include "infrastructure/FrameConfig.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

namespace gameplay {

enum class ProjectileType { Pen, Exam, Pan, Stone, Cup };

class Projectile : public core::Drawable {
public:
    Projectile();

    void init(ProjectileType type, core::Direction dir,
              const sf::Texture& texture,
              infrastructure::FrameConfig& frameConfig,
              const sf::Vector2f& startPos);

    void update(float dt);
    void draw(core::IRenderer& renderer) const override;
    void deactivate();

    bool isActive() const;
    int  getDamage() const;
    float getLifetime() const;
    sf::FloatRect getBounds() const;

private:
    infrastructure::SfmlSprite   m_sprite;
    sf::Vector2f                 m_velocity;
    ProjectileType               m_type = ProjectileType::Pen;
    core::Direction              m_direction = core::Direction::Right;
    int                          m_damage = 0;
    bool                         m_active = false;
    float                        m_lifetime = 0.0f;
};

} // namespace gameplay
