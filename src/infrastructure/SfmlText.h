#pragma once

#include "core/Drawable.h"
#include "core/IRenderer.h"
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Rect2.hpp>
#include <memory>
#include <string>

namespace infrastructure {

/// Adapter: wraps an sf::Text as a core::Drawable.
///
/// In VRSFML, sf::Text is not default-constructible.
/// Text is lazily constructed when setFont() is called.
class SfmlText : public core::Drawable {
public:
    SfmlText() = default;

    void draw(core::IRenderer& renderer) const override;

    void setFont(const sf::Font& font);
    void setString(const std::string& str);
    void setCharacterSize(unsigned size);
    void setFillColor(const sf::Color& color);
    void setBold(bool bold);
    void setItalic(bool italic);
    void setUnderlined(bool underlined);
    void setPosition(float x, float y);
    void setOrigin(float x, float y);

    sf::Rect2f getLocalBounds() const;

    /// Access the underlying sf::Text (created after setFont).
    sf::Text* getSfmlText();

private:
    void ensureText();

    std::unique_ptr<sf::Text> m_text;
    const sf::Font* m_font = nullptr;

    // Stored properties for lazy construction / re-application
    std::string  m_string;
    unsigned     m_charSize   = 30;
    sf::Color    m_fillColor  = sf::Color::White;
    bool         m_bold       = false;
    bool         m_italic     = false;
    bool         m_underlined = false;
    sf::Vec2f    m_position{0.f, 0.f};
    sf::Vec2f    m_origin{0.f, 0.f};
};

} // namespace infrastructure
