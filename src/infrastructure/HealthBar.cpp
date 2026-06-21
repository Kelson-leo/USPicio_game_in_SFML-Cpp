#include "infrastructure/HealthBar.h"
#include "infrastructure/SfmlRenderer.h"
#include <SFML/Graphics/RectangleShape.hpp>

namespace infrastructure {

HealthBar::HealthBar(core::HealthComponent& health, core::Vector2f size)
    : m_health(health)
    , m_position{0.0f, 0.0f}
    , m_size(size) {}

void HealthBar::draw(core::IRenderer& renderer) const {
    if (m_health.isDead()) return;

    auto& sfml = dynamic_cast<SfmlRenderer&>(renderer);

    // Background (red)
    sf::RectangleShape bg{{
        .position = {m_position.x, m_position.y},
        .fillColor = sf::Color::Red,
        .size = {m_size.x, m_size.y}
    }};
    sfml.drawSfml(bg);

    // Fill (green) — proportional to current HP
    float ratio = static_cast<float>(m_health.currentHP) /
                  static_cast<float>(m_health.maxHP);

    sf::RectangleShape fill{{
        .position = {m_position.x, m_position.y},
        .fillColor = sf::Color::Green,
        .size = {m_size.x * ratio, m_size.y}
    }};
    sfml.drawSfml(fill);
}

void HealthBar::setPosition(core::Vector2f position) {
    m_position = position;
}

void HealthBar::setSize(core::Vector2f size) {
    m_size = size;
}

} // namespace infrastructure
