#include <SFML/System/Path.hpp>
#include "infrastructure/AudioManager.h"
#include <iostream>

namespace infrastructure {

std::vector<std::string> AudioManager::buildTrackPaths() {
    return {
        "assets/sounds/music/background_music1.mp3",
        "assets/sounds/music/background_music2.mp3",
        "assets/sounds/music/background_music3.mp3",
        "assets/sounds/music/background_music4.mp3"
    };
}

AudioManager::AudioManager(int initialTrack)
    : m_tracks(buildTrackPaths())
    , m_currentTrackIndex(initialTrack) {
    // Initialise audio context first.
    auto ctx = sf::AudioContext::create();
    if (!ctx.hasValue()) {
        std::cerr << "[AudioManager] WARNING: Cannot create audio context.\n";
        return;
    }
    m_audioContext.emplace(std::move(ctx.value()));

    // Get default playback device.
    auto deviceHandle = sf::AudioContext::getDefaultPlaybackDeviceHandle();
    if (!deviceHandle.hasValue()) {
        std::cerr << "[AudioManager] WARNING: No playback device found.\n";
        return;
    }
    m_device = std::make_unique<sf::PlaybackDevice>(std::move(deviceHandle.value()));

    loadCurrent();
}

void AudioManager::loadCurrent() {
    // Destroy in reverse order: music → reader (music holds ref to reader).
    m_music.reset();
    m_reader.reset();
    m_hasMusic = false;

    if (!m_device) return;

    const auto& path = m_tracks[m_currentTrackIndex];
    auto readerOpt = sf::MusicReader::openFromFile(path);
    if (!readerOpt.hasValue()) {
        std::cerr << "[AudioManager] WARNING: Cannot open "
                  << path << ".\n";
        return;
    }
    m_reader = std::make_unique<sf::MusicReader>(std::move(readerOpt.value()));
    m_music = std::make_unique<sf::Music>(*m_device, *m_reader);
    m_music->setLooping(true);
    m_music->setVolume(m_volume / 100.0f);  // UI scale 0–100 → SFML [0, 1]
    m_hasMusic = true;
}

void AudioManager::play() {
    if (m_music) {
        m_music->play();
    }
}

void AudioManager::stop() {
    if (m_music) {
        m_music->stop();
    }
}

void AudioManager::setMusicVolume(float volume) {
    m_volume = volume;
    if (m_music) {
        m_music->setVolume(volume / 100.0f);  // UI scale 0–100 → SFML [0, 1]
    }
}

void AudioManager::applyConfig(const core::AudioConfig& cfg) {
    setMusicVolume(cfg.musicVolume);
    if (cfg.selectedTrack != m_currentTrackIndex) {
        loadTrack(cfg.selectedTrack);
    }
}

bool AudioManager::hasMusic() const {
    return m_hasMusic;
}

// ── Track selection ─────────────────────────────────────────────

void AudioManager::loadTrack(int index) {
    if (index < 0 || index >= static_cast<int>(m_tracks.size())) return;
    stop();
    m_currentTrackIndex = index;
    loadCurrent();
    play();
}

void AudioManager::nextTrack() {
    int next = (m_currentTrackIndex + 1) % static_cast<int>(m_tracks.size());
    loadTrack(next);
}

void AudioManager::previousTrack() {
    int count = static_cast<int>(m_tracks.size());
    int prev = (m_currentTrackIndex - 1 + count) % count;
    loadTrack(prev);
}

int AudioManager::getTrackCount() const {
    return static_cast<int>(m_tracks.size());
}

int AudioManager::getCurrentTrackIndex() const {
    return m_currentTrackIndex;
}

std::string AudioManager::getCurrentTrackLabel() const {
    return "Track " + std::to_string(m_currentTrackIndex + 1) + "/"
           + std::to_string(m_tracks.size());
}

} // namespace infrastructure
