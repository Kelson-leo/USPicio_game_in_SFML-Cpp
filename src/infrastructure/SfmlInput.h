#pragma once

#include "core/IInputHandler.h"
#include <SFML/Graphics/RenderWindow.hpp>

namespace infrastructure {

/// SFML concrete adapter for core::IInputHandler.
class SfmlInput : public core::IInputHandler {
public:
    /// Window reference (non-const, pollEvent requires it).
    explicit SfmlInput(sf::RenderWindow& window);

    bool pollEvent(core::Event& event) override;
    bool isKeyPressed(core::KeyCode key) const override;

private:
    sf::RenderWindow& m_window;
};

} // namespace infrastructure
