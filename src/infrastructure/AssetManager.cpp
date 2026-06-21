#include <vector>
#include "AssetManager.h"
#include <iostream>

namespace infrastructure {

AssetManager& AssetManager::instance() {
    static AssetManager mgr;
    return mgr;
}

bool AssetManager::loadTexture(const std::string& id,
                               const std::string& path) {
    auto loader = std::make_unique<SfmlTextureLoader>();
    if (loader->loadFromFile(path)) {
        m_cache[id] = std::move(loader);
        return true;
    }

    // File missing or corrupt — create fallback and warn.
    std::cerr << "[AssetManager] WARNING: Could not load '" << path
              << "'. Using fallback for '" << id << "'.\n";
    auto fallback = std::make_unique<SfmlTextureLoader>();
    fallback->setTexture(createFallbackTexture());
    m_cache[id] = std::move(fallback);
    return false;
}

const sf::Texture& AssetManager::getTexture(const std::string& id) const {
    auto it = m_cache.find(id);
    if (it != m_cache.end()) {
        return it->second->getSfmlTexture();
    }
    // Never-loaded ID — return global fallback.
    if (!m_fallback.has_value()) {
        m_fallback = createFallbackTexture();
    }
    return m_fallback.value();
}

void AssetManager::clear() {
    m_cache.clear();
}

sf::Texture AssetManager::createFallbackTexture() {
    // 32x32 solid magenta placeholder (easy to spot missing assets).
    constexpr unsigned S = 32;
    std::vector<sf::base::U8> pixels(S * S * 4, 0xFF);
    for (std::size_t i = 0; i < pixels.size(); i += 4) {
        pixels[i]     = 255; // R
        pixels[i + 1] = 0;   // G
        pixels[i + 2] = 255; // B
        pixels[i + 3] = 255; // A
    }
    auto tex = sf::Texture::create({S, S}).value();
    tex.update(pixels.data());
    return tex;
}

} // namespace infrastructure
