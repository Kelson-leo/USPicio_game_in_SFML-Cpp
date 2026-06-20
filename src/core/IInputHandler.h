#pragma once

namespace core {

// ── Input types (SFML-free) ────────────────────────────────────────

enum class KeyCode {
    Unknown = -1,
    A = 0, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Space,
    Enter,
    Escape,
    Left, Right, Up, Down,
    Count
};

enum class EventType {
    Closed,
    KeyPressed,
    KeyReleased
};

struct Event {
    EventType type = EventType::KeyPressed;
    KeyCode   key  = KeyCode::Unknown;
};

// ── Input port ────────────────────────────────────────────────────

class IInputHandler {
public:
    virtual ~IInputHandler() = default;

    /// Poll next event. Returns false when queue is empty.
    virtual bool pollEvent(Event& event) = 0;

    /// Real-time key state (not buffered).
    virtual bool isKeyPressed(KeyCode key) const = 0;
};

} // namespace core
