#include "infrastructure/AmmoDisplay.h"
#include "infrastructure/SfmlRenderer.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace infrastructure {

AmmoDisplay::AmmoDisplay(core::AmmoComponent& ammo,
                         const sf::Texture& canetaTexture,
                         float scale,
                         float step)
    : m_ammo(ammo)
    , m_canetaTexture(canetaTexture)
    , m_scale(scale)
    , m_step(step)
    , m_position{0.0f, 0.0f} {}

void AmmoDisplay::draw(core::IRenderer& renderer) const {
    auto& sfml = dynamic_cast<SfmlRenderer&>(renderer);

    // Subtle background panel behind the icon row
    const float iconW = m_canetaTexture.getSize().x * m_scale;
    const float iconH = m_canetaTexture.getSize().y * m_scale;
    sf::RectangleShape bg;
    bg.setPosition(m_position.x - 4.0f, m_position.y - 2.0f);
    bg.setSize({m_step * m_ammo.maxAmmo + 4.0f, iconH + 8.0f});
    bg.setFillColor(sf::Color(255, 255, 255, 35));
    sfml.drawSfml(bg);

    for (int i = 0; i < m_ammo.maxAmmo; ++i) {
        float px = m_position.x + i * m_step;
        float py = m_position.y;

        bool used = (i >= m_ammo.currentAmmo);

        // Outline: dark copies in 4 directions
        auto drawOutline = [&](float ox, float oy) {
            sf::Sprite s;
            s.setTexture(m_canetaTexture);
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
        sf::Sprite pen;
        pen.setTexture(m_canetaTexture);
        pen.setScale(m_scale, m_scale);
        pen.setPosition(px, py);

        if (used) {
            pen.setColor(sf::Color(255, 255, 255, 50));
        }

        sfml.drawSfml(pen);

        if (used) {
            float cx = px + iconW / 2.0f;
            float cy = py + iconH / 2.0f;
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

void AmmoDisplay::setPosition(core::Vector2f position) {
    m_position = position;
}

} // namespace infrastructure
