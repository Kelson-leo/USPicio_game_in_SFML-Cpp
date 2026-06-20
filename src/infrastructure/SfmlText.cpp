#include "SfmlText.h"
#include "SfmlRenderer.h"

namespace infrastructure {

void SfmlText::draw(core::IRenderer& renderer) const {
    auto& sfml = dynamic_cast<SfmlRenderer&>(renderer);
    sfml.drawSfml(m_text);
}

void SfmlText::setFont(const sf::Font& font) {
    m_text.setFont(font);
}

void SfmlText::setString(const std::string& str) {
    m_text.setString(str);
}

void SfmlText::setCharacterSize(unsigned size) {
    m_text.setCharacterSize(size);
}

void SfmlText::setFillColor(const sf::Color& color) {
    m_text.setFillColor(color);
}

void SfmlText::setStyle(std::uint32_t style) {
    m_text.setStyle(style);
}

void SfmlText::setPosition(float x, float y) {
    m_text.setPosition({x, y});
}

void SfmlText::setOrigin(float x, float y) {
    m_text.setOrigin({x, y});
}

sf::FloatRect SfmlText::getLocalBounds() const {
    return m_text.getLocalBounds();
}

} // namespace infrastructure
