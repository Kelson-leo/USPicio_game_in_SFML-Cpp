#include "SfmlInput.h"
#include "SfmlConversions.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace infrastructure {

SfmlInput::SfmlInput(sf::Window& window)
    : m_window(window) {
}

bool SfmlInput::pollEvent(core::Event& out) {
    const auto sfEvent = m_window.pollEvent();
    if (!sfEvent.hasValue()) return false;

    // Closed event
    if (sfEvent->is<sf::Event::Closed>()) {
        out.type = core::EventType::Closed;
        out.key  = core::KeyCode::Unknown;
        return true;
    }

    // KeyPressed event
    if (const auto* kp = sfEvent->getIf<sf::Event::KeyPressed>()) {
        out.type = core::EventType::KeyPressed;
        out.key  = toCore(kp->code);
        return true;
    }

    // KeyReleased event
    if (const auto* kr = sfEvent->getIf<sf::Event::KeyReleased>()) {
        out.type = core::EventType::KeyReleased;
        out.key  = toCore(kr->code);
        return true;
    }

    // Skip unhandled event types, recurse to get next
    return pollEvent(out);
}

bool SfmlInput::isKeyPressed(core::KeyCode key) const {
    return sf::Keyboard::isKeyPressed(toSfml(key));
}

} // namespace infrastructure
