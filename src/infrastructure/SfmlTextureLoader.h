#pragma once

#include "core/ITextureLoader.h"
#include <SFML/Graphics.hpp>

namespace infrastructure {

/// SFML adapter for core::ITextureLoader.
/// Owns a single sf::Texture. Used internally by AssetManager.
class SfmlTextureLoader : public core::ITextureLoader {
public:
    bool loadFromFile(const std::string& path) override;

    unsigned int getWidth()  const override;
    unsigned int getHeight() const override;

    /// Access the underlying SFML texture.
    sf::Texture&       getSfmlTexture();
    const sf::Texture& getSfmlTexture() const;

private:
    sf::Texture m_texture;
};

} // namespace infrastructure
