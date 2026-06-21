#include "gameplay/Projectile.h"
#include <algorithm>

namespace gameplay {

Projectile::Projectile() = default;

void Projectile::init(ProjectileType type, core::Direction dir,
                      const sf::Texture& texture,
                      infrastructure::FrameConfig& frameConfig,
                      const sf::Vector2f& startPos) {
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
        if (dir == core::Direction::Right) m_sprite.setScale(-1.0f, 1.0f);
    } else if (type == ProjectileType::Stone) {
        anim = "pedra_left";
        m_velocity = {(dir == core::Direction::Right ? 300.0f : -300.0f), 0.0f};
        m_damage   = 15;
        if (dir == core::Direction::Right) m_sprite.setScale(-1.0f, 1.0f);
    } else if (type == ProjectileType::Cup) {
        anim = "copo_left";
        m_velocity = {(dir == core::Direction::Right ? 200.0f : -200.0f), 0.0f};
        m_damage   = 8;
        if (dir == core::Direction::Right) m_sprite.setScale(-1.0f, 1.0f);
    } else {  // Exam
        anim = "exam_left";
        m_velocity = {(dir == core::Direction::Right ? 250.0f : -250.0f), 0.0f};
        m_damage   = 12;
        if (dir == core::Direction::Right) {
            m_sprite.setScale(-1.0f, 1.0f);
        }
    }

    auto rect = frameConfig.getFrame("projectiles", anim, 0);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));

    // Pen scale makes it visually proportional and enables crouch-hit on capivaras
    if (type == ProjectileType::Pen) {
        m_sprite.setScale(1.5f, 1.5f);
    }

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
    auto pos = m_sprite.getSfmlSprite().getPosition();
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

sf::FloatRect Projectile::getBounds() const {
    return m_sprite.getSfmlSprite().getGlobalBounds();
}

} // namespace gameplay
