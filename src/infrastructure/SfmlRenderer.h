#pragma once

#include "core/IRenderer.h"
#include <SFML/Graphics.hpp>
#include <memory>

namespace infrastructure {

/// SFML concrete adapter for IRenderer.
class SfmlRenderer : public core::IRenderer {
public:
    SfmlRenderer() = default;
    ~SfmlRenderer() override = default;

    bool open(unsigned width, unsigned height,
              const std::string& title) override;

    void close() override;
    bool isOpen() const override;

    void clear(sf::Color color = sf::Color::Black) override;
    void display() override;

    void draw(const sf::Drawable& drawable) override;
    void draw(const sf::Drawable& drawable,
              const sf::RenderStates& states) override;

    sf::RenderWindow& getWindow() override;
    const sf::RenderWindow& getWindow() const override;

private:
    std::unique_ptr<sf::RenderWindow> m_window;
};

} // namespace infrastructure
