#include <SFML/System/Path.hpp>
#include "SfmlTextureLoader.h"

namespace infrastructure {

bool SfmlTextureLoader::loadFromFile(const std::string& path) {
    auto tex = sf::Texture::loadFromFile(path);
    if (tex.hasValue()) {
        m_texture = std::move(tex.value());
        return true;
    }
    return false;
}

unsigned int SfmlTextureLoader::getWidth() const {
    return m_texture ? m_texture->getSize().x : 0u;
}

unsigned int SfmlTextureLoader::getHeight() const {
    return m_texture ? m_texture->getSize().y : 0u;
}

void SfmlTextureLoader::setTexture(sf::Texture tex) {
    m_texture = std::move(tex);
}

bool SfmlTextureLoader::hasTexture() const {
    return m_texture.has_value();
}

sf::Texture& SfmlTextureLoader::getSfmlTexture() {
    return m_texture.value();
}

const sf::Texture& SfmlTextureLoader::getSfmlTexture() const {
    return m_texture.value();
}

} // namespace infrastructure
