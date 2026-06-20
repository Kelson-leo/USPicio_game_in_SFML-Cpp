#pragma once

namespace core {

/// Physical constants for the game world (1080p reference resolution).
constexpr float GROUND_Y      = 880.0f;   // top of the ground tile
constexpr float GROUND_HEIGHT = 200.0f;   // height of the ground tile
constexpr float GRAVITY       = 800.0f;   // pixels/s² downward
constexpr float JUMP_SPEED    = -400.0f;  // initial vertical velocity (negative = up)

} // namespace core
