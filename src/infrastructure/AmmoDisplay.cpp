#include "infrastructure/AmmoDisplay.h"
#include "infrastructure/SfmlRenderer.h"
#include <SFML/Graphics/RenderTarget.hpp>

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

    for (int i = 0; i < m_ammo.currentAmmo; ++i) {
        sf::Sprite pen;
        pen.setTexture(m_canetaTexture);
        pen.setScale(m_scale, m_scale);
        pen.setPosition(m_position.x + i * m_step, m_position.y);
        sfml.drawSfml(pen);
    }
}

void AmmoDisplay::setPosition(core::Vector2f position) {
    m_position = position;
}

} // namespace infrastructure
