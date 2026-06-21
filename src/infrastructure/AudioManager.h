#pragma once

#include "core/AudioConfig.h"
#include <SFML/Audio/Music.hpp>
#include <string>

namespace infrastructure {

/// Manages background music via sf::Music.
/// Follows the adapter pattern — SFML is contained in infrastructure/.
class AudioManager {
public:
    /// @param musicPath  Path to the MP3 file (e.g. assets/sounds/music/background_music.mp3).
    explicit AudioManager(const std::string& musicPath);

    /// Start looping background music.
    void play();

    /// Stop music.
    void stop();

    /// Set music volume (0.0 – 100.0 scale, mapped to SFML 0–100).
    void setMusicVolume(float volume);

    /// Update the internal config snapshot.
    void applyConfig(const core::AudioConfig& cfg);

    /// Whether music was successfully loaded.
    bool hasMusic() const;

private:
    sf::Music m_music;
    bool      m_hasMusic = false;
};

} // namespace infrastructure
