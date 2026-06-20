#pragma once

#include "core/IRenderer.h"
#include "core/IInputHandler.h"
#include "core/DamageConfig.h"
#include "infrastructure/SfmlSprite.h"
#include "infrastructure/SfmlText.h"
#include "infrastructure/FrameConfig.h"
#include "infrastructure/HealthBar.h"
#include "infrastructure/LivesDisplay.h"
#include "infrastructure/AmmoDisplay.h"
#include "gameplay/Player.h"
#include "gameplay/Capivara.h"
#include "gameplay/Professor.h"
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

    // ── Config ────────────────────────────────────────────────────
    core::DamageConfig           m_damageCfg;
    infrastructure::FrameConfig  m_frameConfig;

    // ── Level ─────────────────────────────────────────────────────
    std::unique_ptr<Level> m_currentLevel;

    // ── Menu UI ───────────────────────────────────────────────────
    sf::Font               m_font;
    infrastructure::SfmlText     m_titleText;
    infrastructure::SfmlSprite   m_menuBg;
    std::vector<infrastructure::SfmlSprite> m_menuHearts;

    // ── Entities (created per level) ──────────────────────────────
    std::unique_ptr<Player>    m_player;
    std::vector<Capivara>      m_capivaras;
    std::unique_ptr<Professor> m_professor;

    // ── UI components ─────────────────────────────────────────────
    std::unique_ptr<infrastructure::LivesDisplay> m_livesDisplay;
    std::unique_ptr<infrastructure::AmmoDisplay>  m_ammoDisplay;
    std::unique_ptr<infrastructure::HealthBar>    m_playerHealthBar;
    std::vector<infrastructure::HealthBar>         m_enemyHealthBars;
};

} // namespace gameplay
