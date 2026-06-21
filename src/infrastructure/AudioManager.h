#pragma once

#include "core/AudioConfig.h"
#include <SFML/Audio/Music.hpp>
#include <string>
#include <vector>

namespace infrastructure {

/// Manages background music via sf::Music with multiple track support.
/// Follows the adapter pattern — SFML is contained in infrastructure/.
class AudioManager {
public:
    /// @param initialTrack  Index of the track to load first (0–3).
    explicit AudioManager(int initialTrack = 0);

    /// Start looping background music.
    void play();

    /// Stop music.
    void stop();

    /// Set music volume (0.0 – 100.0 scale, mapped to SFML 0–100).
    void setMusicVolume(float volume);

    /// Update the internal config snapshot.
    void applyConfig(const core::AudioConfig& cfg);

    /// Whether any music was successfully loaded.
    bool hasMusic() const;

    // ── Track selection ─────────────────────────────────────────

    /// Switch to a specific track by index. Falls back if file is missing.
    void loadTrack(int index);

    /// Switch to next track (wraps around).
    void nextTrack();

    /// Switch to previous track (wraps around).
    void previousTrack();

    int getTrackCount() const;
    int getCurrentTrackIndex() const;

    /// Human-readable label for the current track (e.g. "Track 2/4").
    std::string getCurrentTrackLabel() const;

private:
    static std::vector<std::string> buildTrackPaths();

    void loadCurrent();

    sf::Music m_music;
    bool      m_hasMusic = false;

    std::vector<std::string> m_tracks;
    int                      m_currentTrackIndex = 0;
    float                    m_volume = 50.0f;
};

} // namespace infrastructure
