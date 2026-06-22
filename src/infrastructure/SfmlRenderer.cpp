#include "SfmlRenderer.h"
#include "SfmlConversions.h"

namespace infrastructure {

bool SfmlRenderer::open(const core::Vector2u& size,
                        const std::string& title) {
    auto win = sf::RenderWindow::create({
        .size = {size.x, size.y},
        .title = title.c_str()
    });
    if (!win.hasValue()) return false;
    m_window = std::make_unique<sf::RenderWindow>(std::move(win.value()));
    m_window->setVerticalSyncEnabled(true);
    m_isOpen = true;
    return true;
}

void SfmlRenderer::close() {
    m_isOpen = false;
    m_window.reset();
}

bool SfmlRenderer::isOpen() const {
    return m_isOpen && m_window != nullptr;
}

void SfmlRenderer::clear(const core::Color& color) {
    if (m_window) m_window->clear(toSfml(color));
}

void SfmlRenderer::display() {
    if (m_window) m_window->display();
}

void SfmlRenderer::draw(const core::Drawable& drawable) {
    drawable.draw(*this);
}

core::Vector2u SfmlRenderer::getSize() const {
    if (m_window) return toCore(m_window->getSize());
    return {};
}

// ── SFML-specific draw helpers ────────────────────────────────────

void SfmlRenderer::drawSfml(const sf::Shape& shape) {
    if (m_window) m_window->draw(shape);
}

void SfmlRenderer::drawSfml(const sf::Shape& shape,
                             const sf::RenderStates& states) {
    if (m_window) m_window->draw(shape, states);
}

void SfmlRenderer::drawSfml(const sf::Sprite& sprite,
                             const sf::Texture* texture) {
    if (m_window) {
        sf::RenderStates states;
        states.texture = texture;
        m_window->draw(sprite, states);
    }
}

void SfmlRenderer::drawSfml(const sf::Sprite& sprite,
                             const sf::Texture* texture,
                             const sf::RenderStates& states) {
    if (m_window) {
        auto s = states;
        s.texture = texture;
        m_window->draw(sprite, s);
    }
}

void SfmlRenderer::drawSfml(const sf::Text& text) {
    if (m_window) m_window->draw(text);
}

void SfmlRenderer::drawSfml(const std::vector<sf::Vertex>& vertices,
                             sf::PrimitiveType type) {
    if (m_window) m_window->draw(vertices, type);
}

sf::RenderWindow& SfmlRenderer::getSfmlWindow() {
    return *m_window;
}

const sf::RenderWindow& SfmlRenderer::getSfmlWindow() const {
    return *m_window;
}

} // namespace infrastructure
