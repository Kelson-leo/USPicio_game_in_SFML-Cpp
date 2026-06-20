#include "SfmlInput.h"
#include "SfmlConversions.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace infrastructure {

SfmlInput::SfmlInput(sf::RenderWindow& window)
    : m_window(window) {
}

bool SfmlInput::pollEvent(core::Event& out) {
    sf::Event sfEvent;
    if (!m_window.pollEvent(sfEvent)) return false;

    // Translate event type
    switch (sfEvent.type) {
    case sf::Event::Closed:
        out.type = core::EventType::Closed;
        out.key  = core::KeyCode::Unknown;
        break;
    case sf::Event::KeyPressed:
        out.type = core::EventType::KeyPressed;
        out.key  = toCore(sfEvent.key.code);
        break;
    case sf::Event::KeyReleased:
        out.type = core::EventType::KeyReleased;
        out.key  = toCore(sfEvent.key.code);
        break;
    default:
        // Skip non-keyboard / non-close events for now
        return pollEvent(out);
    }

    return true;
}

bool SfmlInput::isKeyPressed(core::KeyCode key) const {
    return sf::Keyboard::isKeyPressed(toSfml(key));
}

} // namespace infrastructure
