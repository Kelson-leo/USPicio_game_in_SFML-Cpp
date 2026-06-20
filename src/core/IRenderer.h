#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace core {

/// Port: abstracts the rendering backend.
class IRenderer {
public:
    virtual ~IRenderer() = default;

    /// Initialise the window.
    virtual bool open(unsigned width, unsigned height, const std::string& title) = 0;

    /// Close the window and release resources.
    virtual void close() = 0;

    /// Is the window still open?
    virtual bool isOpen() const = 0;

    /// Clear the framebuffer.
    virtual void clear(sf::Color color = sf::Color::Black) = 0;

    /// Swap buffers / present.
    virtual void display() = 0;

    /// Draw a drawable object.
    virtual void draw(const sf::Drawable& drawable) = 0;

    /// Draw a drawable with a render state.
    virtual void draw(const sf::Drawable& drawable,
                      const sf::RenderStates& states) = 0;

    /// Expose the underlying window for event polling.
    virtual sf::RenderWindow& getWindow() = 0;

    /// Expose const version.
    virtual const sf::RenderWindow& getWindow() const = 0;
};

} // namespace core
