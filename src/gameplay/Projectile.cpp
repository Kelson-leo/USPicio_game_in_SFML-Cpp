#include "gameplay/Projectile.h"
#include <algorithm>

namespace gameplay {

Projectile::Projectile() = default;

void Projectile::init(ProjectileType type, core::Direction dir,
                      const sf::Texture& texture,
                      infrastructure::FrameConfig& frameConfig,
                      const sf::Vec2f& startPos) {
    m_type      = type;
    m_direction = dir;
    m_active    = true;
    m_lifetime  = 3.0f;

    m_sprite = infrastructure::SfmlSprite(texture);

    // Select animation
    std::string anim;
    if (type == ProjectileType::Pen) {
        anim = (dir == core::Direction::Right) ? "pen_right" : "pen_left";
        m_velocity = {(dir == core::Direction::Right ? 500.0f : -500.0f), 0.0f};
        m_damage   = 20;
    } else if (type == ProjectileType::Pan) {
        anim = "panela_left";
        m_velocity = {(dir == core::Direction::Right ? 250.0f : -250.0f), 0.0f};
        m_damage   = 10;
    } else if (type == ProjectileType::Stone) {
        anim = "pedra_left";
        m_velocity = {(dir == core::Direction::Right ? 300.0f : -300.0f), 0.0f};
        m_damage   = 15;
    } else if (type == ProjectileType::Cup) {
        anim = "copo_left";
        m_velocity = {(dir == core::Direction::Right ? 200.0f : -200.0f), 0.0f};
        m_damage   = 8;
    } else {  // Exam
        anim = "exam_left";
        m_velocity = {(dir == core::Direction::Right ? 250.0f : -250.0f), 0.0f};
        m_damage   = 12;
    }

    auto rect = frameConfig.getFrame("projectiles", anim, 0);
    m_sprite.setTextureRect(sf::Rect2i({rect.position.x, rect.position.y},
                                        {rect.size.x, rect.size.y}));

    // Scale for visibility at 1080p — target sizes proportional to pen (~60×20)
    float scale = 1.0f;
    if (type == ProjectileType::Pen) {
        scale = 1.5f;   // 66×13 native → ~99×20
    } else if (type == ProjectileType::Pan) {
        scale = 0.6f;   // 137×79 native → ~82×47
    } else if (type == ProjectileType::Stone) {
        scale = 0.8f;   // 38×29 native → ~30×23
    } else if (type == ProjectileType::Cup) {
        scale = 0.6f;   // 123×69 native → ~74×41
    } else {  // Exam
        scale = 1.0f;   // 62×67 native → ~62×67 (already large enough)
    }

    // Apply scale with horizontal flip for right-facing projectiles
    // Pen uses separate _right frames; others mirror the _left frame
    float xScale = (dir == core::Direction::Right && type != ProjectileType::Pen)
        ? -scale : scale;
    m_sprite.setScale(xScale, scale);

    m_sprite.setPosition(startPos.x, startPos.y);
}

void Projectile::update(float dt) {
    if (!m_active) return;

    m_lifetime -= dt;
    if (m_lifetime <= 0.0f) {
        m_active = false;
        return;
    }

    // Move by velocity
    auto pos = m_sprite.getSfmlSprite().position;
    pos += m_velocity * dt;
    m_sprite.setPosition(pos.x, pos.y);
}

void Projectile::draw(core::IRenderer& renderer) const {
    if (!m_active) return;
    m_sprite.draw(renderer);
}

void Projectile::deactivate() {
    m_active = false;
}

bool Projectile::isActive() const {
    return m_active;
}

int Projectile::getDamage() const {
    return m_damage;
}

float Projectile::getLifetime() const {
    return m_lifetime;
}

sf::Rect2f Projectile::getBounds() const {
    return m_sprite.getSfmlSprite().getGlobalBounds();
}

} // namespace gameplay
