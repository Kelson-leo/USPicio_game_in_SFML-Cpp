#include "infrastructure/LivesDisplay.h"
#include "infrastructure/SfmlRenderer.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>

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
    sf::RectangleShape bg;
    bg.setPosition(m_position.x - 4.0f, m_position.y - 2.0f);
    bg.setSize({m_step * m_lives.maxLives + 4.0f, iconH + 8.0f});
    bg.setFillColor(sf::Color(255, 255, 255, 35));
    sfml.drawSfml(bg);

    for (int i = 0; i < m_lives.maxLives; ++i) {
        float px = m_position.x + i * m_step;
        float py = m_position.y;

        bool lost = (i >= m_lives.currentLives);

        // Outline: draw dark copies offset in 4 directions
        auto drawOutline = [&](float ox, float oy) {
            sf::Sprite s;
            s.setTexture(m_heartTexture);
            s.setScale(m_scale, m_scale);
            s.setPosition(px + ox, py + oy);
            s.setColor(sf::Color(20, 20, 20, 200));
            sfml.drawSfml(s);
        };
        drawOutline(-1,  0);
        drawOutline( 1,  0);
        drawOutline( 0, -1);
        drawOutline( 0,  1);

        // Main icon
        sf::Sprite heart;
        heart.setTexture(m_heartTexture);
        heart.setScale(m_scale, m_scale);
        heart.setPosition(px, py);

        if (lost) {
            heart.setColor(sf::Color(255, 255, 255, 50));
        }

        sfml.drawSfml(heart);

        if (lost) {
            float cx = px + 10.0f;
            float cy = py + 10.0f;
            sf::VertexArray cross(sf::Lines, 4);
            cross[0].position = {cx - 7, cy - 7};
            cross[0].color    = sf::Color(220, 20, 20);
            cross[1].position = {cx + 7, cy + 7};
            cross[1].color    = sf::Color(220, 20, 20);
            cross[2].position = {cx + 7, cy - 7};
            cross[2].color    = sf::Color(220, 20, 20);
            cross[3].position = {cx - 7, cy + 7};
            cross[3].color    = sf::Color(220, 20, 20);
            sfml.drawSfml(cross);
        }
    }
}

void LivesDisplay::setPosition(core::Vector2f position) {
    m_position = position;
}

} // namespace infrastructure
