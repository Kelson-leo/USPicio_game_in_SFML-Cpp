#pragma once

namespace core {

/// Audio volume configuration (0–100 scale).
/// Stored in core/ to remain SFML-free.
struct AudioConfig {
    float musicVolume = 50.0f;   // 0 = mute, 100 = max
    float effectVolume = 50.0f;  // placeholder for future SFX
};

} // namespace core
