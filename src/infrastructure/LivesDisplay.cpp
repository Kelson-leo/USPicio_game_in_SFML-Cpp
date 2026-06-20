#include "infrastructure/LivesDisplay.h"
#include "infrastructure/SfmlRenderer.h"
#include <SFML/Graphics/RenderTarget.hpp>

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

    for (int i = 0; i < m_lives.currentLives; ++i) {
        sf::Sprite heart;
        heart.setTexture(m_heartTexture);
        heart.setScale(m_scale, m_scale);
        heart.setPosition(m_position.x + i * m_step, m_position.y);
        sfml.drawSfml(heart);
    }
}

void LivesDisplay::setPosition(core::Vector2f position) {
    m_position = position;
}

} // namespace infrastructure
