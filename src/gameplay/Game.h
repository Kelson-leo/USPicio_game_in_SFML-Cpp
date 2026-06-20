#pragma once

#include "core/IRenderer.h"
#include "core/IInputHandler.h"
#include "infrastructure/SfmlSprite.h"
#include "infrastructure/SfmlText.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

namespace gameplay {

class Level;

/// Top-level game orchestrator. Owns the main loop.
class Game {
public:
    Game(core::IRenderer& renderer, core::IInputHandler& input);
    ~Game();

    void run();

private:
    static constexpr float FIXED_DT  = 1.0f / 60.0f;
    static constexpr float MAX_FRAME = 0.25f;

    void processInput();
    void update(float deltaTime);
    void render();

    // ── State ─────────────────────────────────────────────────────
    enum class State { Menu, Playing, Paused };
    void setState(State newState);

    void loadLevel(int phaseNumber);

    // ── Dependencies ──────────────────────────────────────────────
    core::IRenderer&     m_renderer;
    core::IInputHandler&  m_input;

    State m_state   = State::Menu;
    bool  m_running = true;

    // ── Level ─────────────────────────────────────────────────────
    std::unique_ptr<Level> m_currentLevel;

    // ── Menu UI ───────────────────────────────────────────────────
    sf::Font               m_font;
    infrastructure::SfmlText     m_titleText;
    infrastructure::SfmlSprite   m_menuBg;

    // ── Hearts (visual only for now) ──────────────────────────────
    std::vector<infrastructure::SfmlSprite> m_hearts;
};

} // namespace gameplay
