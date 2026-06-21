#pragma once

#include "core/Drawable.h"
#include "core/IRenderer.h"
#include <SFML/Graphics.hpp>

namespace infrastructure {

/// Adapter: wraps an sf::Sprite as a core::Drawable.
///
/// This is the bridge between the SFML rendering backend and the
/// domain's Drawable abstraction.  It resolves TD-01 by moving
/// the SFML-specific draw logic out of gameplay/ and into
/// infrastructure/.
class SfmlSprite : public core::Drawable {
public:
    SfmlSprite() = default;

    explicit SfmlSprite(const sf::Texture& texture);
    SfmlSprite(const sf::Texture& texture, const sf::IntRect& rect);

    void draw(core::IRenderer& renderer) const override;

    // ── Passthrough to sf::Sprite ────────────────────────────────
    void setTexture(const sf::Texture& texture, bool resetRect = false);
    void setTextureRect(const sf::IntRect& rect);
    void setPosition(float x, float y);
    void setScale(float x, float y);
    void setColor(const sf::Color& color);

    sf::Sprite&       getSfmlSprite();
    sf::Sprite&       getSfmlSprite() const;  // mutable sprite, safe to return non-const

    sf::Color getColor() const;
    sf::Vector2f getPosition() const;

private:
    mutable sf::Sprite m_sprite;  // mutable: visual-only changes in const draw
};

} // namespace infrastructure
