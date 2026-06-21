#pragma once

#include "core/IRenderer.h"
#include "core/PhysicsConstants.h"
#include "infrastructure/SfmlSprite.h"
#include <memory>
#include <string>

namespace gameplay {

/// Represents a single game level (phase). Owns the background sprite,
/// loaded via AssetManager from the given path.
class Level {
public:
    /// @param bgPath   Path to the background texture.
    /// @param groundY  Ground Y for this level.
    explicit Level(const std::string& bgPath, float groundY);

    /// Draw the background.
    void draw(core::IRenderer& renderer);

    /// Y-coordinate of the ground surface.
    float getGroundY() const;

private:
    std::string m_bgPath;
    std::string m_bgId;     ///< AssetManager key derived from the path
    float       m_groundY;

    infrastructure::SfmlSprite m_background;
};

} // namespace gameplay
