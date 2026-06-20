#include "Level.h"
#include "infrastructure/AssetManager.h"

namespace gameplay {

Level::Level(int phaseNumber)
    : m_phaseNumber(phaseNumber) {

    // ── Select asset IDs per phase ───────────────────────────────
    switch (phaseNumber) {
    case 1:
        m_bgId   = "background_fase1";
        m_tileId = "tile_grama";
        break;
    case 2:
        m_bgId   = "background_fase2";
        m_tileId = "tile_madeira";
        break;
    case 3:
        m_bgId   = "background_fase3";
        m_tileId = "tile_grama";   // Reitoria uses grass too
        break;
    default:
        m_bgId   = "background_fase1";
        m_tileId = "tile_grama";
        break;
    }

    auto& assets = infrastructure::AssetManager::instance();

    // Load (or fallback) each texture by its canonical path from CLAUDE.md.
    assets.loadTexture("background_fase1",
                       "assets/backgrounds/fase1_patio.png");
    assets.loadTexture("background_fase2",
                       "assets/backgrounds/fase2_biblioteca.png");
    assets.loadTexture("background_fase3",
                       "assets/backgrounds/fase3_reitoria.png");
    assets.loadTexture("tile_grama",
                       "assets/tiles/tile_grama.png");
    assets.loadTexture("tile_madeira",
                       "assets/tiles/tile_madeira.png");

    m_background.setTexture(assets.getTexture(m_bgId), true);
    m_groundTile.setTexture(assets.getTexture(m_tileId), true);
}

void Level::draw(core::IRenderer& renderer) {
    const auto winSize = renderer.getSize();

    // ── Background: stretch to fill the window ───────────────────
    const auto* bgTex = m_background.getSfmlSprite().getTexture();
    if (bgTex && bgTex->getSize().x > 0 && bgTex->getSize().y > 0) {
        m_background.setScale(
            static_cast<float>(winSize.x) / bgTex->getSize().x,
            static_cast<float>(winSize.y) / bgTex->getSize().y);
    }
    m_background.setPosition(0.0f, 0.0f);
    renderer.draw(m_background);

    // ── Ground tile: temporarily disabled (background has its own floor) ─
    // m_groundTile.setScale(...)
    // m_groundTile.setPosition(0.0f, core::GROUND_Y);
    // renderer.draw(m_groundTile);
}

float Level::getGroundY() const {
    return core::GROUND_Y;
}

} // namespace gameplay
