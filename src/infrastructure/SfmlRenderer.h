#pragma once

#include "core/IRenderer.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace infrastructure {

/// SFML concrete adapter for core::IRenderer.
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

    // ── SFML-specific helpers (temporary — see CLAUDE.md) ─────────
    /// Draw an SFML drawable directly. Used by gameplay/ until
    /// proper game-object rendering is in place.
    void drawSfml(const sf::Drawable& d);
    void drawSfml(const sf::Drawable& d, const sf::RenderStates& states);

    /// Access the underlying SFML window (needed for font loading etc.)
    sf::RenderWindow& getSfmlWindow();
    const sf::RenderWindow& getSfmlWindow() const;

private:
    std::unique_ptr<sf::RenderWindow> m_window;
};

} // namespace infrastructure
