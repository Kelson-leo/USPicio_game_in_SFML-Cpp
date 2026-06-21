#include "infrastructure/AmmoDisplay.h"
#include "infrastructure/SfmlRenderer.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <vector>

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
    sf::RectangleShape bg{{
        .position = {m_position.x - 4.0f, m_position.y - 2.0f},
        .size = {m_step * m_ammo.maxAmmo + 4.0f, iconH + 8.0f},
        .fillColor = sf::Color{255, 255, 255, 35}
    }};
    sfml.drawSfml(bg);

    for (int i = 0; i < m_ammo.maxAmmo; ++i) {
        float px = m_position.x + i * m_step;
        float py = m_position.y;

        bool used = (i >= m_ammo.currentAmmo);

        // Outline: dark copies in 4 directions
        auto drawOutline = [&](float ox, float oy) {
            sf::Sprite s{
                .position = {px + ox, py + oy},
                .scale = {m_scale, m_scale},
                .color = sf::Color{20, 20, 20, 200}
            };
            sfml.drawSfml(s, &m_canetaTexture);
        };
        drawOutline(-1,  0);
        drawOutline( 1,  0);
        drawOutline( 0, -1);
        drawOutline( 0,  1);

        // Main icon
        sf::Color penColor = used ? sf::Color{255, 255, 255, 50} : sf::Color::White;
        sf::Sprite pen{
            .position = {px, py},
            .scale = {m_scale, m_scale},
            .color = penColor
        };
        sfml.drawSfml(pen, &m_canetaTexture);

        if (used) {
            float cx = px + iconW / 2.0f;
            float cy = py + iconH / 2.0f;
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

void AmmoDisplay::setPosition(core::Vector2f position) {
    m_position = position;
}

} // namespace infrastructure
