#include "SfmlSprite.h"
#include "SfmlRenderer.h"

namespace infrastructure {

SfmlSprite::SfmlSprite(const sf::Texture& texture)
    : m_sprite(texture) {
}

SfmlSprite::SfmlSprite(const sf::Texture& texture,
                       const sf::IntRect& rect)
    : m_sprite(texture, rect) {
}

void SfmlSprite::draw(core::IRenderer& renderer) const {
    auto& sfml = dynamic_cast<SfmlRenderer&>(renderer);
    sfml.drawSfml(m_sprite);
}

void SfmlSprite::setTexture(const sf::Texture& texture,
                            bool resetRect) {
    m_sprite.setTexture(texture, resetRect);
}

void SfmlSprite::setTextureRect(const sf::IntRect& rect) {
    m_sprite.setTextureRect(rect);
}

void SfmlSprite::setPosition(float x, float y) {
    m_sprite.setPosition({x, y});
}

void SfmlSprite::setScale(float x, float y) {
    m_sprite.setScale({x, y});
}

void SfmlSprite::setColor(const sf::Color& color) {
    m_sprite.setColor(color);
}

sf::Sprite& SfmlSprite::getSfmlSprite() {
    return m_sprite;
}

sf::Sprite& SfmlSprite::getSfmlSprite() const {
    return m_sprite;
}

sf::Color SfmlSprite::getColor() const {
    return m_sprite.getColor();
}

sf::Vector2f SfmlSprite::getPosition() const {
    return m_sprite.getPosition();
}

} // namespace infrastructure
