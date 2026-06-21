#pragma once

#include "core/Drawable.h"
#include "core/IRenderer.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vec2.hpp>
#include <SFML/System/Rect2.hpp>

namespace infrastructure {

/// Adapter: wraps an sf::Sprite as a core::Drawable.
///
/// In VRSFML (SFML 3.x), sf::Sprite does not store a texture reference.
/// The texture is passed at draw time.  This adapter stores the texture
/// pointer separately and passes it to drawSfml().
class SfmlSprite : public core::Drawable {
public:
    SfmlSprite() = default;

    explicit SfmlSprite(const sf::Texture& texture);
    SfmlSprite(const sf::Texture& texture, const sf::Rect2i& rect);

    void draw(core::IRenderer& renderer) const override;

    // ── Passthrough to sf::Sprite ────────────────────────────────
    void setTexture(const sf::Texture& texture, bool resetRect = false);
    void setTextureRect(const sf::Rect2i& rect);
    void setPosition(float x, float y);
    void setScale(float x, float y);
    void setColor(const sf::Color& color);

    sf::Sprite&       getSfmlSprite();
    sf::Sprite&       getSfmlSprite() const;  // mutable for draw-time tweaks

    const sf::Texture* getTexture() const;  // for draw-time texture binding

    sf::Color   getColor()    const;
    sf::Vec2f   getPosition() const;

private:
    mutable sf::Sprite  m_sprite;
    const sf::Texture*  m_texture = nullptr;
};

} // namespace infrastructure
