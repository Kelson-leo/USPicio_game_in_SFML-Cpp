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
    sf::Texture temp;
    createFallback(temp);
    fallback->getSfmlTexture().swap(temp);
    m_cache[id] = std::move(fallback);
    return false;
}

const sf::Texture& AssetManager::getTexture(const std::string& id) const {
    auto it = m_cache.find(id);
    if (it != m_cache.end()) {
        return it->second->getSfmlTexture();
    }
    // Never-loaded ID — return global fallback.
    if (m_fallback.getSize().x == 0) {
        createFallback(m_fallback);
    }
    return m_fallback;
}

void AssetManager::clear() {
    m_cache.clear();
}

void AssetManager::createFallback(sf::Texture& tex) {
    // 32x32 solid magenta placeholder (easy to spot missing assets).
    constexpr unsigned S = 32;
    std::vector<sf::Uint8> pixels(S * S * 4, 0xFF);
    for (std::size_t i = 0; i < pixels.size(); i += 4) {
        pixels[i]     = 255; // R
        pixels[i + 1] = 0;   // G
        pixels[i + 2] = 255; // B
        pixels[i + 3] = 255; // A
    }
    tex.create(S, S);
    tex.update(pixels.data());
}

} // namespace infrastructure
