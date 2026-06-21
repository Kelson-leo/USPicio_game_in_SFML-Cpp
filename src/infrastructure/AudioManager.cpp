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
    loadCurrent();
}

void AudioManager::loadCurrent() {
    m_hasMusic = false;
    const auto& path = m_tracks[m_currentTrackIndex];
    if (m_music.openFromFile(path)) {
        m_hasMusic = true;
        m_music.setLoop(true);
        m_music.setVolume(m_volume);
    } else {
        std::cerr << "[AudioManager] WARNING: Cannot open "
                  << path << ".\n";
    }
}

void AudioManager::play() {
    if (m_hasMusic) {
        m_music.play();
    }
}

void AudioManager::stop() {
    m_music.stop();
}

void AudioManager::setMusicVolume(float volume) {
    m_volume = volume;
    if (m_hasMusic) {
        m_music.setVolume(volume);
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
    m_music.stop();
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
