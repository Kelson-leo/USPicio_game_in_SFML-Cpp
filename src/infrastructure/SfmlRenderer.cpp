#include "SfmlRenderer.h"
#include "SfmlConversions.h"

namespace infrastructure {

bool SfmlRenderer::open(const core::Vector2u& size,
                        const std::string& title) {
    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(size.x, size.y), title, sf::Style::Default);
    m_window->setVerticalSyncEnabled(false);
    return m_window->isOpen();
}

void SfmlRenderer::close() {
    if (m_window) {
        m_window->close();
        m_window.reset();
    }
}

bool SfmlRenderer::isOpen() const {
    return m_window && m_window->isOpen();
}

void SfmlRenderer::clear(const core::Color& color) {
    if (m_window) m_window->clear(toSfml(color));
}

void SfmlRenderer::display() {
    if (m_window) m_window->display();
}

void SfmlRenderer::draw(const core::Drawable& drawable) {
    // The core::Drawable's own draw() will call back into this
    // renderer (double dispatch). Concrete drawable subclasses
    // that wrap SFML objects will use drawSfml() internally.
    drawable.draw(*this);
}

core::Vector2u SfmlRenderer::getSize() const {
    if (m_window) return toCore(m_window->getSize());
    return {};
}

// ── SFML-specific helpers ────────────────────────────────────────

void SfmlRenderer::drawSfml(const sf::Drawable& d) {
    if (m_window) m_window->draw(d);
}

void SfmlRenderer::drawSfml(const sf::Drawable& d,
                            const sf::RenderStates& states) {
    if (m_window) m_window->draw(d, states);
}

sf::RenderWindow& SfmlRenderer::getSfmlWindow() {
    return *m_window;
}

const sf::RenderWindow& SfmlRenderer::getSfmlWindow() const {
    return *m_window;
}

} // namespace infrastructure
