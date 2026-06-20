#include "SfmlTextureLoader.h"

namespace infrastructure {

bool SfmlTextureLoader::loadFromFile(const std::string& path) {
    return m_texture.loadFromFile(path);
}

unsigned int SfmlTextureLoader::getWidth() const {
    return m_texture.getSize().x;
}

unsigned int SfmlTextureLoader::getHeight() const {
    return m_texture.getSize().y;
}

sf::Texture& SfmlTextureLoader::getSfmlTexture() {
    return m_texture;
}

const sf::Texture& SfmlTextureLoader::getSfmlTexture() const {
    return m_texture;
}

} // namespace infrastructure
