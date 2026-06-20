#pragma once

#include "core/Drawable.h"
#include "core/IRenderer.h"
#include <SFML/Graphics.hpp>
#include <string>

namespace infrastructure {

/// Adapter: wraps an sf::Text as a core::Drawable.
class SfmlText : public core::Drawable {
public:
    SfmlText() = default;

    void draw(core::IRenderer& renderer) const override;

    void setFont(const sf::Font& font);
    void setString(const std::string& str);
    void setCharacterSize(unsigned size);
    void setFillColor(const sf::Color& color);
    void setStyle(std::uint32_t style);
    void setPosition(float x, float y);
    void setOrigin(float x, float y);

    sf::FloatRect getLocalBounds() const;

private:
    sf::Text m_text;
};

} // namespace infrastructure
