#include "SfmlSprite.h"
#include "SfmlRenderer.h"

namespace infrastructure {

SfmlSprite::SfmlSprite(const sf::Texture& texture)
    : m_texture(&texture) {
    m_sprite.textureRect = texture.getRect();
}

SfmlSprite::SfmlSprite(const sf::Texture& texture,
                       const sf::Rect2i& rect)
    : m_texture(&texture) {
    m_sprite.textureRect = sf::Rect2f{{static_cast<float>(rect.position.x),
                                        static_cast<float>(rect.position.y)},
                                       {static_cast<float>(rect.size.x),
                                        static_cast<float>(rect.size.y)}};
}

void SfmlSprite::draw(core::IRenderer& renderer) const {
    auto& sfml = dynamic_cast<SfmlRenderer&>(renderer);
    sfml.drawSfml(m_sprite, m_texture);
}

void SfmlSprite::setTexture(const sf::Texture& texture,
                            bool resetRect) {
    m_texture = &texture;
    if (resetRect) {
        m_sprite.textureRect = texture.getRect();
    }
}

void SfmlSprite::setTextureRect(const sf::Rect2i& rect) {
    m_sprite.textureRect = sf::Rect2f{{static_cast<float>(rect.position.x),
                                        static_cast<float>(rect.position.y)},
                                       {static_cast<float>(rect.size.x),
                                        static_cast<float>(rect.size.y)}};
}

void SfmlSprite::setPosition(float x, float y) {
    m_sprite.position = {x, y};
}

void SfmlSprite::setScale(float x, float y) {
    m_sprite.scale = {x, y};
}

void SfmlSprite::setColor(const sf::Color& color) {
    m_sprite.color = color;
}

sf::Sprite& SfmlSprite::getSfmlSprite() {
    return m_sprite;
}

sf::Sprite& SfmlSprite::getSfmlSprite() const {
    return m_sprite;
}

const sf::Texture* SfmlSprite::getTexture() const {
    return m_texture;
}

sf::Color SfmlSprite::getColor() const {
    return m_sprite.color;
}

sf::Vec2f SfmlSprite::getPosition() const {
    return m_sprite.position;
}

} // namespace infrastructure
