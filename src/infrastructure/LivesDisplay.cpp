#include "infrastructure/LivesDisplay.h"
#include "infrastructure/SfmlRenderer.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <vector>

namespace infrastructure {

LivesDisplay::LivesDisplay(core::LivesComponent& lives,
                           const sf::Texture& heartTexture,
                           float scale,
                           float step)
    : m_lives(lives)
    , m_heartTexture(heartTexture)
    , m_scale(scale)
    , m_step(step)
    , m_position{0.0f, 0.0f} {}

void LivesDisplay::draw(core::IRenderer& renderer) const {
    auto& sfml = dynamic_cast<SfmlRenderer&>(renderer);

    // Subtle background panel behind the icon row
    const float iconW = 20.0f;   // heart ≈20px
    const float iconH = 20.0f;
    sf::RectangleShape bg{{
        .position = {m_position.x - 4.0f, m_position.y - 2.0f},
        .fillColor = sf::Color{255, 255, 255, 35},
        .size = {m_step * m_lives.maxLives + 4.0f, iconH + 8.0f}
    }};
    sfml.drawSfml(bg);

    for (int i = 0; i < m_lives.maxLives; ++i) {
        float px = m_position.x + i * m_step;
        float py = m_position.y;

        bool lost = (i >= m_lives.currentLives);

        // Outline: draw dark copies offset in 4 directions
        const sf::Rect2f heartRect = m_heartTexture.getRect();
        auto drawOutline = [&](float ox, float oy) {
            sf::Sprite s{
                .position = {px + ox, py + oy},
                .scale = {m_scale, m_scale},
                .textureRect = heartRect,
                .color = sf::Color{20, 20, 20, 200}
            };
            sfml.drawSfml(s, &m_heartTexture);
        };
        drawOutline(-1,  0);
        drawOutline( 1,  0);
        drawOutline( 0, -1);
        drawOutline( 0,  1);

        // Main icon
        sf::Color heartColor = lost ? sf::Color{255, 255, 255, 50} : sf::Color::White;
        sf::Sprite heart{
            .position = {px, py},
            .scale = {m_scale, m_scale},
            .textureRect = heartRect,
            .color = heartColor
        };
        sfml.drawSfml(heart, &m_heartTexture);

        if (lost) {
            float cx = px + 10.0f;
            float cy = py + 10.0f;
            std::vector<sf::Vertex> cross = {
                {{cx - 7, cy - 7}, sf::Color{220, 20, 20}},
                {{cx + 7, cy + 7}, sf::Color{220, 20, 20}},
                {{cx + 7, cy - 7}, sf::Color{220, 20, 20}},
                {{cx - 7, cy + 7}, sf::Color{220, 20, 20}}
            };
            sfml.drawSfml(cross, sf::PrimitiveType::Lines);
        }
    }
}

void LivesDisplay::setPosition(core::Vector2f position) {
    m_position = position;
}

} // namespace infrastructure
