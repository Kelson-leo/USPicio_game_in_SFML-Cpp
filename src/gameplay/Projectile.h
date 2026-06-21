#pragma once

#include "core/Drawable.h"
#include "core/Direction.h"
#include "infrastructure/SfmlSprite.h"
#include "infrastructure/FrameConfig.h"
#include <SFML/System/Rect2.hpp>
#include <SFML/System/Vec2.hpp>
#include <string>

namespace gameplay {

enum class ProjectileType { Pen, Exam, Pan, Stone, Cup };

class Projectile : public core::Drawable {
public:
    Projectile();

    void init(ProjectileType type, core::Direction dir,
              const sf::Texture& texture,
              infrastructure::FrameConfig& frameConfig,
              const sf::Vec2f& startPos);

    void update(float dt);
    void draw(core::IRenderer& renderer) const override;
    void deactivate();

    bool isActive() const;
    int  getDamage() const;
    float getLifetime() const;
    sf::Rect2f getBounds() const;

private:
    infrastructure::SfmlSprite   m_sprite;
    sf::Vec2f                 m_velocity;
    ProjectileType               m_type = ProjectileType::Pen;
    core::Direction              m_direction = core::Direction::Right;
    int                          m_damage = 0;
    bool                         m_active = false;
    float                        m_lifetime = 0.0f;
};

} // namespace gameplay
