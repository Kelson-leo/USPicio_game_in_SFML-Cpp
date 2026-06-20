#include "SfmlInput.h"

namespace infrastructure {

SfmlInput::SfmlInput(const sf::RenderWindow& window)
    : m_window(window) {
}

bool SfmlInput::pollEvent(sf::Event& event) {
    // Unfortunately, SFML's pollEvent is a non-const method on Window.
    // We need to const_cast here because we store a const ref.
    return const_cast<sf::RenderWindow&>(m_window).pollEvent(event);
}

} // namespace infrastructure
