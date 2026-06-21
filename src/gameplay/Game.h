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
#include "core/AudioConfig.h"
#include "infrastructure/AudioManager.h"
#include "gameplay/PhaseConfig.h"
#include "gameplay/Player.h"
#include "gameplay/Capivara.h"
#include "gameplay/Professor.h"
#include "gameplay/Projectile.h"
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
    enum class State { Menu, Playing, Paused, GameOver, Victory };
    enum class MenuPage { Main, Options, Info };
    void setState(State newState);

    void loadLevel(int phaseIndex);
    void restartGame();
    void restartPhase();
    void advancePhase();

    // ── Menu helpers ──────────────────────────────────────────────
    void handleMenuInput(const core::Event& event);
    void handleOptionsInput(const core::Event& event);
    void handleInfoInput(const core::Event& event);
    void handlePauseInput(const core::Event& event);
    void handleGameOverInput(const core::Event& event);
    void renderMenu();
    void renderOptions();
    void renderInfo();
    void renderPauseMenu();
    void renderGameOver();
    void renderVictory();
    void updateMenuTextColors();

    // ── Dependencies ──────────────────────────────────────────────
    core::IRenderer&      m_renderer;
    core::IInputHandler&  m_input;

    State    m_state    = State::Menu;
    MenuPage m_menuPage = MenuPage::Main;
    bool     m_running  = true;

    int m_menuSelection    = 0;  // 0=Start, 1=Options, 2=Info, 3=Restart
    int m_optionsSelection = 0;  // 0=MusicVol, 1=SFXVol, 2=Back
    int m_infoSelection    = 0;  // 0=Back
    int m_pauseSelection   = 0;  // 0=Resume, 1=Restart, 2=Quit to Menu

    // ── Config ────────────────────────────────────────────────────
    core::DamageConfig           m_damageCfg;
    infrastructure::FrameConfig  m_frameConfig;
    PhaseConfig                  m_phaseConfig;
    core::AudioConfig            m_audioCfg;

    // ── Level ─────────────────────────────────────────────────────
    std::unique_ptr<Level> m_currentLevel;
    int m_currentPhase = 0;  // 0-based index into PhaseConfig

    // ── Menu UI ───────────────────────────────────────────────────
    sf::Font    m_font;
    infrastructure::SfmlText m_titleText;
    infrastructure::SfmlSprite m_menuBg;
    std::vector<infrastructure::SfmlSprite> m_menuHearts;
    sf::RectangleShape m_menuOverlay;

    std::vector<infrastructure::SfmlText> m_menuItems; // "Start", "Restart", "Info"

    // ── Options screen UI ──────────────────────────────────────────
    sf::RectangleShape m_optionsOverlay;
    infrastructure::SfmlText m_optionsTitle;
    infrastructure::SfmlText m_optionsMusicLabel;
    infrastructure::SfmlText m_optionsMusicBar;
    infrastructure::SfmlText m_optionsSfxLabel;
    infrastructure::SfmlText m_optionsSfxBar;
    infrastructure::SfmlText m_optionsHint;
    infrastructure::SfmlText m_optionsBack;

    std::string buildSliderBar(float value) const;

    // ── Info screen UI ────────────────────────────────────────────
    sf::RectangleShape m_infoOverlay;
    infrastructure::SfmlText m_infoTitle;
    infrastructure::SfmlText m_infoDev;
    infrastructure::SfmlText m_infoCopyright;
    infrastructure::SfmlText m_infoControls;
    infrastructure::SfmlText m_infoBack;

    // ── Pause menu UI ─────────────────────────────────────────────
    sf::RectangleShape m_pauseOverlay;
    std::vector<infrastructure::SfmlText> m_pauseItems; // "Resume", "Restart", "Quit to Menu"

    // ── Game Over / Victory UI ────────────────────────────────────
    sf::RectangleShape m_gameOverOverlay;
    infrastructure::SfmlText m_gameOverTitle;
    infrastructure::SfmlText m_gameOverSubtext;

    // ── Entities (created per level) ──────────────────────────────
    std::unique_ptr<Player>    m_player;
    std::vector<Capivara>      m_capivaras;
    std::unique_ptr<Professor> m_professor;

    // ── Projectiles ───────────────────────────────────────────────
    std::vector<std::unique_ptr<Projectile>> m_projectiles;

    // ── Audio ─────────────────────────────────────────────────────
    std::unique_ptr<infrastructure::AudioManager> m_audio;

    // ── UI components ─────────────────────────────────────────────
    std::unique_ptr<infrastructure::LivesDisplay> m_livesDisplay;
    std::unique_ptr<infrastructure::AmmoDisplay>  m_ammoDisplay;
    std::unique_ptr<infrastructure::HealthBar>    m_playerHealthBar;
    std::vector<infrastructure::HealthBar>         m_enemyHealthBars;
};

} // namespace gameplay
