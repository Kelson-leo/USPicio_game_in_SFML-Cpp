#pragma once

#include "core/Drawable.h"
#include "core/AmmoComponent.h"
#include "core/IRenderer.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace infrastructure {

/// Draws pen icons for the player's remaining ammo.
class AmmoDisplay : public core::Drawable {
public:
    AmmoDisplay(core::AmmoComponent& ammo,
                const sf::Texture& canetaTexture,
                float scale = 0.04f,
                float step = 30.0f);

    void draw(core::IRenderer& renderer) const override;

    void setPosition(core::Vector2f position);

private:
    core::AmmoComponent& m_ammo;
    const sf::Texture&   m_canetaTexture;
    float                m_scale;
    float                m_step;
    core::Vector2f       m_position;
};

} // namespace infrastructure
