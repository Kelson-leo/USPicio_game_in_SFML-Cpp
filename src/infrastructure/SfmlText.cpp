#include "SfmlText.h"
#include "SfmlRenderer.h"

namespace infrastructure {

void SfmlText::ensureText() {
    if (m_text || !m_font) return;
    m_text = std::make_unique<sf::Text>(*m_font, sf::Text::Data{});
    m_text->setString(m_string.c_str());
    m_text->setCharacterSize(m_charSize);
    m_text->setFillColor(m_fillColor);
    m_text->setBold(m_bold);
    m_text->setItalic(m_italic);
    m_text->setUnderlined(m_underlined);
    m_text->position = m_position;
    m_text->origin    = m_origin;
}

void SfmlText::draw(core::IRenderer& renderer) const {
    if (!m_text) return;
    auto& sfml = dynamic_cast<SfmlRenderer&>(renderer);
    sfml.drawSfml(*m_text);
}

void SfmlText::setFont(const sf::Font& font) {
    m_font = &font;
    m_text.reset(); // Force reconstruction with new font
    ensureText();
}

void SfmlText::setString(const std::string& str) {
    m_string = str;
    if (m_text) m_text->setString(str.c_str());
}

void SfmlText::setCharacterSize(unsigned size) {
    m_charSize = size;
    if (m_text) m_text->setCharacterSize(size);
}

void SfmlText::setFillColor(const sf::Color& color) {
    m_fillColor = color;
    if (m_text) m_text->setFillColor(color);
}

void SfmlText::setBold(bool bold) {
    m_bold = bold;
    if (m_text) m_text->setBold(bold);
}

void SfmlText::setItalic(bool italic) {
    m_italic = italic;
    if (m_text) m_text->setItalic(italic);
}

void SfmlText::setUnderlined(bool underlined) {
    m_underlined = underlined;
    if (m_text) m_text->setUnderlined(underlined);
}

void SfmlText::setPosition(float x, float y) {
    m_position = {x, y};
    if (m_text) m_text->position = m_position;
}

void SfmlText::setOrigin(float x, float y) {
    m_origin = {x, y};
    if (m_text) m_text->origin = m_origin;
}

sf::Rect2f SfmlText::getLocalBounds() const {
    if (m_text) return m_text->getLocalBounds();
    return {};
}

sf::Text* SfmlText::getSfmlText() {
    ensureText();
    return m_text.get();
}

} // namespace infrastructure
