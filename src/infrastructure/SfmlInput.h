#pragma once

#include "core/IInputHandler.h"
#include <SFML/Graphics/RenderWindow.hpp>

namespace infrastructure {

/// SFML concrete adapter for IInputHandler.
class SfmlInput : public core::IInputHandler {
public:
    /// Must be called with the active window reference.
    explicit SfmlInput(const sf::RenderWindow& window);

    bool pollEvent(sf::Event& event) override;

private:
    const sf::RenderWindow& m_window;
};

} // namespace infrastructure
