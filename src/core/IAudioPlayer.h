#pragma once

#include <string>

namespace core {

/// Port: abstracts audio playback.
class IAudioPlayer {
public:
    virtual ~IAudioPlayer() = default;

    /// Play a sound file (by asset path).
    virtual void playSound(const std::string& assetPath) = 0;

    /// Start background music (looping).
    virtual void playMusic(const std::string& assetPath) = 0;

    /// Stop music.
    virtual void stopMusic() = 0;

    /// Set master volume [0-100].
    virtual void setVolume(float volume) = 0;
};

} // namespace core
