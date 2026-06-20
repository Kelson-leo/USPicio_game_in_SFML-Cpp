#include "SfmlRenderer.h"

namespace infrastructure {

bool SfmlRenderer::open(unsigned width, unsigned height,
                        const std::string& title) {
    m_window = std::make_unique<sf::RenderWindow>(
        sf::VideoMode({width, height}), title, sf::Style::Default);
    // Avoid vertical sync for fixed-timestep predictability.
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

void SfmlRenderer::clear(sf::Color color) {
    if (m_window) m_window->clear(color);
}

void SfmlRenderer::display() {
    if (m_window) m_window->display();
}

void SfmlRenderer::draw(const sf::Drawable& drawable) {
    if (m_window) m_window->draw(drawable);
}

void SfmlRenderer::draw(const sf::Drawable& drawable,
                        const sf::RenderStates& states) {
    if (m_window) m_window->draw(drawable, states);
}

sf::RenderWindow& SfmlRenderer::getWindow() {
    return *m_window;
}

const sf::RenderWindow& SfmlRenderer::getWindow() const {
    return *m_window;
}

} // namespace infrastructure
