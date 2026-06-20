#pragma once

#include "core/IRenderer.h"
#include "core/IInputHandler.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace infrastructure {

// ── Color ──────────────────────────────────────────────────────────

inline sf::Color toSfml(const core::Color& c) {
    return sf::Color(c.r, c.g, c.b, c.a);
}

inline core::Color toCore(const sf::Color& c) {
    return {c.r, c.g, c.b, c.a};
}

// ── Vector2f ───────────────────────────────────────────────────────

inline sf::Vector2f toSfml(const core::Vector2f& v) {
    return {v.x, v.y};
}

inline core::Vector2f toCore(const sf::Vector2f& v) {
    return {v.x, v.y};
}

// ── Vector2u ───────────────────────────────────────────────────────

inline sf::Vector2u toSfml(const core::Vector2u& v) {
    return {v.x, v.y};
}

inline core::Vector2u toCore(const sf::Vector2u& v) {
    return {v.x, v.y};
}

// ── FloatRect ──────────────────────────────────────────────────────

inline sf::FloatRect toSfml(const core::FloatRect& r) {
    return {r.left, r.top, r.width, r.height};
}

inline core::FloatRect toCore(const sf::FloatRect& r) {
    return {r.left, r.top, r.width, r.height};
}

// ── KeyCode <-> sf::Keyboard::Key ──────────────────────────────────
// Letters A-Z map to sf::Keyboard::A (0) through sf::Keyboard::Z (25).
// Special keys use an explicit lookup.

inline sf::Keyboard::Key toSfml(core::KeyCode code) {
    if (code >= core::KeyCode::A && code <= core::KeyCode::Z) {
        return static_cast<sf::Keyboard::Key>(
            static_cast<int>(code) - static_cast<int>(core::KeyCode::A));
    }
    switch (code) {
    case core::KeyCode::Space:  return sf::Keyboard::Key::Space;
    case core::KeyCode::Enter:  return sf::Keyboard::Key::Enter;
    case core::KeyCode::Escape: return sf::Keyboard::Key::Escape;
    case core::KeyCode::Left:   return sf::Keyboard::Key::Left;
    case core::KeyCode::Right:  return sf::Keyboard::Key::Right;
    case core::KeyCode::Up:     return sf::Keyboard::Key::Up;
    case core::KeyCode::Down:   return sf::Keyboard::Key::Down;
    default:                    return sf::Keyboard::Key::Unknown;
    }
}

inline core::KeyCode toCore(sf::Keyboard::Key key) {
    if (key >= sf::Keyboard::Key::A && key <= sf::Keyboard::Key::Z) {
        return static_cast<core::KeyCode>(
            static_cast<int>(core::KeyCode::A) + static_cast<int>(key));
    }
    switch (key) {
    case sf::Keyboard::Key::Space:  return core::KeyCode::Space;
    case sf::Keyboard::Key::Enter:  return core::KeyCode::Enter;
    case sf::Keyboard::Key::Escape: return core::KeyCode::Escape;
    case sf::Keyboard::Key::Left:   return core::KeyCode::Left;
    case sf::Keyboard::Key::Right:  return core::KeyCode::Right;
    case sf::Keyboard::Key::Up:     return core::KeyCode::Up;
    case sf::Keyboard::Key::Down:   return core::KeyCode::Down;
    default:                        return core::KeyCode::Unknown;
    }
}

} // namespace infrastructure
