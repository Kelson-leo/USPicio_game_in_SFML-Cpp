#pragma once

#include "SfmlTextureLoader.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace infrastructure {

/// Centralised texture cache (Singleton).
///
/// Thread-unsafe by design (games are single-threaded).
/// Consumers should prefer constructor injection of AssetManager&
/// for testability; instance() is a convenience.
class AssetManager {
public:
    static AssetManager& instance();

    /// Load a texture from disk and cache it by ID.
    /// If the file is missing, creates a fallback placeholder texture
    /// (solid magenta) and logs a warning.
    /// @return true if the actual file was loaded, false if fallback was used.
    bool loadTexture(const std::string& id, const std::string& path);

    /// Retrieve a cached texture by ID.
    /// If the ID was never loaded, returns a fallback placeholder.
    const sf::Texture& getTexture(const std::string& id) const;

    /// Release all cached textures.
    void clear();

private:
    AssetManager() = default;

    static void createFallback(sf::Texture& tex);

    std::unordered_map<std::string, std::unique_ptr<SfmlTextureLoader>> m_cache;
    mutable sf::Texture m_fallback; // returned for unknown IDs / failed loads
};

} // namespace infrastructure
