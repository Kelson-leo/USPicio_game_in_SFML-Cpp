#pragma once

#include "core/Drawable.h"
#include "core/LivesComponent.h"
#include "core/IRenderer.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace infrastructure {

/// Draws heart icons for the player's remaining lives.
class LivesDisplay : public core::Drawable {
public:
    LivesDisplay(core::LivesComponent& lives,
                 const sf::Texture& heartTexture,
                 float scale = 0.04f,
                 float step = 30.0f);

    void draw(core::IRenderer& renderer) const override;

    void setPosition(core::Vector2f position);

private:
    core::LivesComponent& m_lives;
    const sf::Texture&    m_heartTexture;
    float                 m_scale;
    float                 m_step;
    core::Vector2f        m_position;
};

} // namespace infrastructure
