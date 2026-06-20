#pragma once

#include "core/IRenderer.h"
#include "core/IInputHandler.h"
#include <SFML/Graphics.hpp>
#include <string>

namespace gameplay {

/// Top-level game orchestrator. Owns the main loop.
class Game {
public:
    Game(core::IRenderer& renderer, core::IInputHandler& input);

    /// Start the main loop. Blocks until the window closes.
    void run();

private:
    // ── Fixed timestep constants ──────────────────────────────────
    static constexpr float FIXED_DT   = 1.0f / 60.0f;
    static constexpr float MAX_FRAME  = 0.25f;

    // ── Loop phases ───────────────────────────────────────────────
    void processInput();
    void update(float deltaTime);
    void render();

    // ── Internal state placeholder ────────────────────────────────
    enum class State { Menu, Playing, Paused };
    void setState(State newState);

    // ── Dependencies ──────────────────────────────────────────────
    core::IRenderer&     m_renderer;
    core::IInputHandler&  m_input;

    // ── State ─────────────────────────────────────────────────────
    State m_state   = State::Menu;
    bool  m_running = true;

    // ── Menu visuals (SFML objects — temporary, will become
    //     core::Drawable game-objects in later sprints) ────────────
    sf::Font           m_font;
    sf::Text           m_menuText;
    sf::RectangleShape m_menuBg;
};

} // namespace gameplay
