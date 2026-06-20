#pragma once

#include <SFML/Window/Event.hpp>

namespace core {

/// Port: abstracts input processing.
class IInputHandler {
public:
    virtual ~IInputHandler() = default;

    /// Poll the next event from the queue.
    /// Returns false when no events remain.
    virtual bool pollEvent(sf::Event& event) = 0;
};

} // namespace core
