#pragma once

#include "core/ITextureLoader.h"
#include <SFML/Graphics/Texture.hpp>
#include <optional>

namespace infrastructure {

/// SFML adapter for core::ITextureLoader.
/// Owns a single sf::Texture. Used internally by AssetManager.
class SfmlTextureLoader : public core::ITextureLoader {
public:
    bool loadFromFile(const std::string& path) override;

    unsigned int getWidth()  const override;
    unsigned int getHeight() const override;

    /// Move a pre-created texture into this loader (e.g. fallback).
    void setTexture(sf::Texture tex);

    /// Check whether a texture has been loaded or set.
    bool hasTexture() const;

    /// Access the underlying SFML texture.
    /// Must only be called when hasTexture() is true.
    sf::Texture&       getSfmlTexture();
    const sf::Texture& getSfmlTexture() const;

private:
    std::optional<sf::Texture> m_texture;
};

} // namespace infrastructure
