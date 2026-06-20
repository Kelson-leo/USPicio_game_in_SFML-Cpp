#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace infrastructure {

/// Loads sprite-sheet frame rectangles from a JSON configuration file.
///
/// Each character has named animations; each animation is a list of
/// {x, y, w, h} frames.  Frame widths may differ across animations
/// (e.g. a punch frame is wider than an idle frame).
///
/// Usage:
///   FrameConfig cfg;
///   cfg.loadFromFile("assets/config/frames.json");
///   sf::IntRect r = cfg.getFrame("player", "walk", 0);
class FrameConfig {
public:
    struct Frame {
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
    };
    using Animation = std::vector<Frame>;
    using AnimMap   = std::unordered_map<std::string, Animation>;
    using CharMap   = std::unordered_map<std::string, AnimMap>;

    /// Parse a JSON file.
    bool loadFromFile(const std::string& path);

    /// Number of frames in an animation.  Returns 0 for unknown keys.
    std::size_t frameCount(const std::string& character,
                           const std::string& animation) const;

    /// Get a frame rectangle.  Out-of-range index wraps to last valid
    /// frame. Missing character/animation returns {0,0,0,0}.
    sf::IntRect getFrame(const std::string& character,
                         const std::string& animation,
                         std::size_t index) const;

private:
    CharMap m_characters;
};

} // namespace infrastructure
