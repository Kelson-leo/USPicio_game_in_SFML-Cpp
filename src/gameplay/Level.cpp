#include "Level.h"
#include "infrastructure/AssetManager.h"

namespace gameplay {

Level::Level(const std::string& bgPath)
    : m_bgPath(bgPath) {

    // Derive a simple AssetManager key from the path (strip dirs + ext).
    // e.g. "assets/backgrounds/fase1_patio.png" → "bg_fase1_patio"
    auto start = bgPath.rfind('/');
    if (start == std::string::npos) start = 0; else ++start;
    auto end   = bgPath.rfind('.');
    m_bgId = "bg_" + bgPath.substr(start, end - start);

    auto& assets = infrastructure::AssetManager::instance();
    assets.loadTexture(m_bgId, bgPath);

    m_background.setTexture(assets.getTexture(m_bgId), true);
}

void Level::draw(core::IRenderer& renderer) {
    const auto winSize = renderer.getSize();

    // Background: stretch to fill the window
    const auto* bgTex = m_background.getSfmlSprite().getTexture();
    if (bgTex && bgTex->getSize().x > 0 && bgTex->getSize().y > 0) {
        m_background.setScale(
            static_cast<float>(winSize.x) / bgTex->getSize().x,
            static_cast<float>(winSize.y) / bgTex->getSize().y);
    }
    m_background.setPosition(0.0f, 0.0f);
    renderer.draw(m_background);
}

float Level::getGroundY() const {
    return core::GROUND_Y;
}

} // namespace gameplay
