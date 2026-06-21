#pragma once

#include "core/AudioConfig.h"
#include <SFML/Audio/AudioContext.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/MusicReader.hpp>
#include <SFML/Audio/PlaybackDevice.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace infrastructure {

/// Manages background music via VRSFML sf::Music with multiple tracks.
///
/// VRSFML requires Music to be constructed with a PlaybackDevice and
/// MusicReader (both move-only).  We store them as members alongside
/// the Music itself and manage lifetimes explicitly.
class AudioManager {
public:
    /// Creates audio context, playback device, and loads the initial track.
    /// @param initialTrack  Index of the track to load first (0–3).
    explicit AudioManager(int initialTrack = 0);

    /// Start looping background music.
    void play();

    /// Stop music.
    void stop();

    /// Set music volume (0.0 – 100.0 scale, internally mapped to [0, 1]).
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

    // Must be declared before m_device, m_reader, m_music (lifetime order).
    // Actually AudioContext is optional and static methods work without it,
    // but we keep it alive to hold the audio subsystem.
    std::optional<sf::AudioContext>    m_audioContext;

    std::unique_ptr<sf::PlaybackDevice> m_device;
    std::unique_ptr<sf::MusicReader>    m_reader;
    std::unique_ptr<sf::Music>          m_music;

    bool m_hasMusic = false;

    std::vector<std::string> m_tracks;
    int                      m_currentTrackIndex = 0;
    float                    m_volume = 50.0f;  // UI scale 0–100
};

} // namespace infrastructure
