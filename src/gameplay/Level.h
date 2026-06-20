#pragma once

#include "core/IRenderer.h"
#include "core/PhysicsConstants.h"
#include "infrastructure/SfmlSprite.h"
#include <memory>
#include <string>

namespace gameplay {

/// Represents a single game level (phase). Owns the background
/// and ground-tile sprites, loaded via AssetManager.
class Level {
public:
    /// @param phaseNumber  1, 2, or 3.
    explicit Level(int phaseNumber);

    /// Draw the background and the ground tile.
    void draw(core::IRenderer& renderer);

    /// Y-coordinate of the ground surface.
    float getGroundY() const;

private:
    int m_phaseNumber;

    infrastructure::SfmlSprite m_background;
    infrastructure::SfmlSprite m_groundTile;

    /// Asset IDs for the current phase.
    std::string m_bgId;
    std::string m_tileId;
};

} // namespace gameplay
