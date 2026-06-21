#pragma once

#include "core/IRenderer.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <memory>
#include <vector>

namespace infrastructure {

/// SFML concrete adapter for core::IRenderer.
///
/// VRSFML (SFML 3.x) removes the abstract sf::Drawable interface,
/// so drawSfml() provides overloads for each concrete type.
class SfmlRenderer : public core::IRenderer {
public:
    SfmlRenderer() = default;
    ~SfmlRenderer() override = default;

    // ── core::IRenderer interface ─────────────────────────────────
    bool open(const core::Vector2u& size,
              const std::string& title) override;

    void close() override;
    bool isOpen() const override;

    void clear(const core::Color& color) override;
    void display() override;

    void draw(const core::Drawable& drawable) override;

    core::Vector2u getSize() const override;

    // ── SFML-specific draw helpers ───────────────────────────────
    void drawSfml(const sf::Shape& shape);
    void drawSfml(const sf::Shape& shape, const sf::RenderStates& states);
    void drawSfml(const sf::Sprite& sprite, const sf::Texture* texture);
    void drawSfml(const sf::Sprite& sprite, const sf::Texture* texture,
                  const sf::RenderStates& states);
    void drawSfml(const sf::Text& text);
    void drawSfml(const std::vector<sf::Vertex>& vertices,
                  sf::PrimitiveType type);

    /// Access the underlying SFML window.
    sf::RenderWindow& getSfmlWindow();
    const sf::RenderWindow& getSfmlWindow() const;

private:
    std::unique_ptr<sf::RenderWindow> m_window;
    bool m_isOpen = false;
};

} // namespace infrastructure
