#pragma once

#include "core/Drawable.h"
#include <cstdint>
#include <string>

namespace core {

// ── Geometry / colour types (SFML-free) ───────────────────────────

struct Color {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;

    static constexpr Color Black()   { return {0,   0,   0,   255}; }
    static constexpr Color White()   { return {255, 255, 255, 255}; }
    static constexpr Color Red()     { return {255, 0,   0,   255}; }
    static constexpr Color Green()   { return {0,   255, 0,   255}; }
    static constexpr Color Blue()    { return {0,   0,   255, 255}; }
    static constexpr Color Transparent() { return {0, 0, 0, 0}; }
};

struct Vector2f {
    float x = 0.0f;
    float y = 0.0f;
};

struct Vector2u {
    unsigned int x = 0;
    unsigned int y = 0;
};

struct FloatRect {
    float left   = 0.0f;
    float top    = 0.0f;
    float width  = 0.0f;
    float height = 0.0f;
};

// ── Renderer port ─────────────────────────────────────────────────

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual bool open(const Vector2u& size, const std::string& title) = 0;

    virtual void close() = 0;

    virtual bool isOpen() const = 0;

    virtual void clear(const Color& color = Color::Black()) = 0;

    virtual void display() = 0;

    /// Draw a core::Drawable.
    virtual void draw(const Drawable& drawable) = 0;

    /// Current window size.
    virtual Vector2u getSize() const = 0;
};

} // namespace core
