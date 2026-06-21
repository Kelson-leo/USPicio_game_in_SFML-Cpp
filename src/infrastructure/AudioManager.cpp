#include "infrastructure/AudioManager.h"
#include <iostream>

namespace infrastructure {

AudioManager::AudioManager(const std::string& musicPath) {
    if (m_music.openFromFile(musicPath)) {
        m_hasMusic = true;
        m_music.setLoop(true);
        m_music.setVolume(50.0f);  // default = 50%
    } else {
        std::cerr << "[AudioManager] WARNING: Cannot open "
                  << musicPath << ". Music disabled.\n";
        m_hasMusic = false;
    }
}

void AudioManager::play() {
    if (m_hasMusic) {
        m_music.play();
    }
}

void AudioManager::stop() {
    if (m_hasMusic) {
        m_music.stop();
    }
}

void AudioManager::setMusicVolume(float volume) {
    if (m_hasMusic) {
        m_music.setVolume(volume);  // SFML uses 0–100, same as our scale
    }
}

void AudioManager::applyConfig(const core::AudioConfig& cfg) {
    setMusicVolume(cfg.musicVolume);
}

bool AudioManager::hasMusic() const {
    return m_hasMusic;
}

} // namespace infrastructure
