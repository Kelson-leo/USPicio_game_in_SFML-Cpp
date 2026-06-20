#include "FrameConfig.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace infrastructure {

bool FrameConfig::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[FrameConfig] Cannot open '" << path << "'\n";
        return false;
    }

    nlohmann::json root;
    try {
        file >> root;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "[FrameConfig] JSON parse error: " << e.what() << '\n';
        return false;
    }

    m_characters.clear();
    for (const auto& [charName, animations] : root.items()) {
        AnimMap animMap;
        for (const auto& [animName, frames] : animations.items()) {
            Animation anim;
            for (const auto& f : frames) {
                anim.push_back({
                    f.value("x", 0),
                    f.value("y", 0),
                    f.value("w", 0),
                    f.value("h", 0)
                });
            }
            animMap[animName] = std::move(anim);
        }
        m_characters[charName] = std::move(animMap);
    }
    return true;
}

std::size_t FrameConfig::frameCount(const std::string& character,
                                    const std::string& animation) const {
    auto charIt = m_characters.find(character);
    if (charIt == m_characters.end()) return 0;
    auto animIt = charIt->second.find(animation);
    if (animIt == charIt->second.end()) return 0;
    return animIt->second.size();
}

sf::IntRect FrameConfig::getFrame(const std::string& character,
                                  const std::string& animation,
                                  std::size_t index) const {
    auto charIt = m_characters.find(character);
    if (charIt == m_characters.end()) return {};

    auto animIt = charIt->second.find(animation);
    if (animIt == charIt->second.end()) return {};

    const auto& frames = animIt->second;
    if (frames.empty()) return {};

    if (index >= frames.size()) index = frames.size() - 1;

    const auto& f = frames[index];
    return sf::IntRect({f.x, f.y}, {f.w, f.h});
}

} // namespace infrastructure
