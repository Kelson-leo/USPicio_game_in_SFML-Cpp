#include "Game.h"
#include "Level.h"
#include "infrastructure/AssetManager.h"
#include "infrastructure/SfmlRenderer.h"
#include "core/PhysicsConstants.h"
#include <cmath>
#include <iostream>

namespace gameplay {

Game::~Game() = default;

// ────────────────────────────────────────────────────────────────────
Game::Game(core::IRenderer& renderer, core::IInputHandler& input)
    : m_renderer(renderer)
    , m_input(input) {

    // ── Font ─────────────────────────────────────────────────────
    if (!m_font.loadFromFile("assets/fonts/PressStart2P.ttf")) {
        std::cerr << "[Game] WARNING: Could not load font. "
                     "Text will not render.\n";
    }

    // ── Frame config ─────────────────────────────────────────────
    m_frameConfig.loadFromFile("assets/config/frames.json");

    // ── Phase config ─────────────────────────────────────────────
    if (!m_phaseConfig.loadFromFile("assets/config/fases.json")) {
        std::cerr << "[Game] WARNING: Could not load phase config. "
                     "Game will have no phases.\n";
    }

    // ── Preload menu / common assets ─────────────────────────────
    auto& assets = infrastructure::AssetManager::instance();
    assets.loadTexture("background_fase1",
                       "assets/backgrounds/fase1_patio.png");
    assets.loadTexture("heart",
                       "assets/ui/heart.png");
    assets.loadTexture("caneta",
                       "assets/projectiles/caneta.png");
    assets.loadTexture("caneta_disp",
                       "assets/projectiles/caneta_disp.png");

    // ── Preload all phase backgrounds ────────────────────────────
    for (int i = 0; i < m_phaseConfig.size(); ++i) {
        const auto& bgPath = m_phaseConfig.getBackground(i);
        // Derive a key matching Level's scheme
        auto start = bgPath.rfind('/');
        if (start == std::string::npos) start = 0; else ++start;
        auto end = bgPath.rfind('.');
        std::string bgId = "bg_" + bgPath.substr(start, end - start);
        assets.loadTexture(bgId, bgPath);
    }

    // ── Menu background ──────────────────────────────────────────
    m_menuBg.setTexture(assets.getTexture("background_fase1"), true);

    // ── Title ────────────────────────────────────────────────────
    m_titleText.setFont(m_font);
    m_titleText.setString("USPICIO");
    m_titleText.setCharacterSize(48);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);

    // ── Menu overlay ─────────────────────────────────────────────
    m_menuOverlay.setSize({380.0f, 240.0f});
    m_menuOverlay.setFillColor(sf::Color(20, 20, 40, 220));

    // ── Audio ────────────────────────────────────────────────────
    m_audio = std::make_unique<infrastructure::AudioManager>(
        m_audioCfg.selectedTrack);
    m_audio->applyConfig(m_audioCfg);
    m_audio->play();

    // ── Menu items ───────────────────────────────────────────────
    const char* labels[] = {"Start", "Options", "Info", "Restart"};
    for (const auto* lbl : labels) {
        auto& item = m_menuItems.emplace_back();
        item.setFont(m_font);
        item.setString(lbl);
        item.setCharacterSize(36);
        item.setFillColor(sf::Color::White);
    }

    // ── Options overlay ──────────────────────────────────────────
    m_optionsOverlay.setSize({700.0f, 430.0f});
    m_optionsOverlay.setFillColor(sf::Color(30, 30, 40, 245));

    m_optionsTitle.setFont(m_font);
    m_optionsTitle.setString("Options");
    m_optionsTitle.setCharacterSize(40);
    m_optionsTitle.setFillColor(sf::Color::White);
    m_optionsTitle.setStyle(sf::Text::Bold);

    m_optionsMusicLabel.setFont(m_font);
    m_optionsMusicLabel.setString("Music Volume:");
    m_optionsMusicLabel.setCharacterSize(24);
    m_optionsMusicLabel.setFillColor(sf::Color(200, 200, 255));

    m_optionsMusicBar.setFont(m_font);
    m_optionsMusicBar.setCharacterSize(18);
    m_optionsMusicBar.setFillColor(sf::Color::White);

    m_optionsSfxLabel.setFont(m_font);
    m_optionsSfxLabel.setString("Sound Effects:");
    m_optionsSfxLabel.setCharacterSize(24);
    m_optionsSfxLabel.setFillColor(sf::Color(200, 200, 255));

    m_optionsSfxBar.setFont(m_font);
    m_optionsSfxBar.setCharacterSize(18);
    m_optionsSfxBar.setFillColor(sf::Color::White);

    m_optionsTrackLabel.setFont(m_font);
    m_optionsTrackLabel.setString("Background Music:");
    m_optionsTrackLabel.setCharacterSize(24);
    m_optionsTrackLabel.setFillColor(sf::Color(200, 200, 255));

    m_optionsTrackSelector.setFont(m_font);
    m_optionsTrackSelector.setCharacterSize(20);
    m_optionsTrackSelector.setFillColor(sf::Color::White);

    m_optionsHint.setFont(m_font);
    m_optionsHint.setString("Left/Right Arrow to adjust volume");
    m_optionsHint.setCharacterSize(16);
    m_optionsHint.setFillColor(sf::Color(150, 150, 150));

    m_optionsBack.setFont(m_font);
    m_optionsBack.setString("Back (Esc / Enter)");
    m_optionsBack.setCharacterSize(28);
    m_optionsBack.setFillColor(sf::Color::Yellow);

    // ── Info overlay (larger, darker) ─────────────────────────────
    m_infoOverlay.setSize({800.0f, 600.0f});
    m_infoOverlay.setFillColor(sf::Color(30, 30, 40, 245));

    m_infoTitle.setFont(m_font);
    m_infoTitle.setString("Info");
    m_infoTitle.setCharacterSize(40);
    m_infoTitle.setFillColor(sf::Color::White);
    m_infoTitle.setStyle(sf::Text::Bold);

    m_infoDev.setFont(m_font);
    m_infoDev.setString("Developed by Kelson");
    m_infoDev.setCharacterSize(24);
    m_infoDev.setFillColor(sf::Color(200, 200, 255));

    m_infoCopyright.setFont(m_font);
    m_infoCopyright.setString("(c) 2026 Kelson");
    m_infoCopyright.setCharacterSize(20);
    m_infoCopyright.setFillColor(sf::Color(160, 160, 160));

    m_infoControls.setFont(m_font);
    m_infoControls.setString(
        "Controls:\n\n"
        "  Arrow Keys  -  Move\n\n"
        "  Space       -  Jump\n\n"
        "  Z           -  Punch\n\n"
        "  X           -  Throw Pen\n\n"
        "  C           -  Defend\n\n"
        "  Down Arrow  -  Crouch\n\n"
        "  Enter       -  Select / Start\n\n"
        "  Escape      -  Pause / Back");
    m_infoControls.setCharacterSize(18);
    m_infoControls.setFillColor(sf::Color(220, 220, 220));

    m_infoBack.setFont(m_font);
    m_infoBack.setString("Back (Esc / Enter)");
    m_infoBack.setCharacterSize(28);
    m_infoBack.setFillColor(sf::Color::Yellow);

    // ── Pause overlay ────────────────────────────────────────────
    m_pauseOverlay.setSize({380.0f, 280.0f});
    m_pauseOverlay.setFillColor(sf::Color(20, 20, 40, 230));

    const char* pauseOpts[] = {"Resume", "Options", "Restart", "Quit to Menu"};
    for (const auto* lbl : pauseOpts) {
        auto& item = m_pauseItems.emplace_back();
        item.setFont(m_font);
        item.setString(lbl);
        item.setCharacterSize(30);
        item.setFillColor(sf::Color::White);
    }

    // ── Game Over / Victory overlay (shared) ─────────────────────
    m_gameOverOverlay.setSize({600.0f, 300.0f});
    m_gameOverOverlay.setFillColor(sf::Color(80, 10, 10, 235));

    m_gameOverTitle.setFont(m_font);
    m_gameOverTitle.setCharacterSize(56);
    m_gameOverTitle.setFillColor(sf::Color::Red);
    m_gameOverTitle.setStyle(sf::Text::Bold);

    m_gameOverSubtext.setFont(m_font);
    m_gameOverSubtext.setString("Press Enter to return to menu");
    m_gameOverSubtext.setCharacterSize(22);
    m_gameOverSubtext.setFillColor(sf::Color(200, 200, 200));

    // ── Menu hearts (static, 5 top-left) ─────────────────────────
    constexpr float HEART_SCALE = 20.0f / 500.0f;
    constexpr float HEART_STEP  = 30.0f;
    const auto& heartTex = assets.getTexture("heart");
    for (int i = 0; i < 5; ++i) {
        auto& h = m_menuHearts.emplace_back(heartTex);
        h.setScale(HEART_SCALE, HEART_SCALE);
        h.setPosition(10.0f + i * HEART_STEP, 10.0f);
    }

    updateMenuTextColors();
}

// ────────────────────────────────────────────────────────────────────
void Game::run() {
    sf::Clock clock;
    float accumulator = 0.0f;

    while (m_running && m_renderer.isOpen()) {
        const float frameTime = clock.restart().asSeconds();
        accumulator += (frameTime > MAX_FRAME) ? MAX_FRAME : frameTime;

        processInput();

        while (accumulator >= FIXED_DT) {
            update(FIXED_DT);
            accumulator -= FIXED_DT;
        }

        render();
    }
}

// ────────────────────────────────────────────────────────────────────
void Game::processInput() {
    core::Event event;
    while (m_input.pollEvent(event)) {
        if (event.type == core::EventType::Closed) {
            m_running = false;
            m_renderer.close();
            return;
        }

        if (event.type == core::EventType::KeyPressed) {
            // ── Global Escape handling ──────────────────────────
            if (event.key == core::KeyCode::Escape) {
                if (m_state == State::Playing) {
                    setState(State::Paused);
                    m_pauseSelection = 0;
                    continue;
                }
                if (m_state == State::Paused && m_menuPage == MenuPage::Options) {
                    m_optionsFromPause = false;
                    m_menuPage = MenuPage::Main;  // back to pause menu
                    continue;
                }
                if (m_state == State::Paused) {
                    setState(State::Playing);  // resume
                    continue;
                }
                if (m_state == State::Menu && m_menuPage != MenuPage::Main) {
                    m_menuPage = MenuPage::Main;
                    continue;
                }
                // In Main menu → exit
                m_running = false;
                m_renderer.close();
                return;
            }

            if (m_state == State::Menu) {
                if (m_menuPage == MenuPage::Main) {
                    handleMenuInput(event);
                } else if (m_menuPage == MenuPage::Options) {
                    handleOptionsInput(event);
                } else if (m_menuPage == MenuPage::Info) {
                    handleInfoInput(event);
                }
            } else if (m_state == State::Paused) {
                if (m_menuPage == MenuPage::Options) {
                    handleOptionsInput(event);
                } else {
                    handlePauseInput(event);
                }
            } else if (m_state == State::GameOver || m_state == State::Victory) {
                handleGameOverInput(event);
            }
        }
    }

    // ── Real-time input (Playing state only) ────────────────────
    if (m_state == State::Playing && m_player && !m_player->health.isDead()) {
        bool moving = false;

        if (m_input.isKeyPressed(core::KeyCode::Left)) {
            m_player->setDirection(core::Direction::Left);
            m_player->setAnimation("walk");
            m_player->moveLeft(FIXED_DT);
            moving = true;
        }

        if (m_input.isKeyPressed(core::KeyCode::Right)) {
            m_player->setDirection(core::Direction::Right);
            m_player->setAnimation("walk");
            m_player->moveRight(FIXED_DT);
            moving = true;
        }

        if (m_input.isKeyPressed(core::KeyCode::Down)) {
            m_player->setCrouching(true);
        } else {
            m_player->setCrouching(false);
        }

        if (m_input.isKeyPressed(core::KeyCode::Space) ||
            m_input.isKeyPressed(core::KeyCode::Up)) {
            if (m_player->isOnGround() && !m_player->isCrouching()) {
                m_player->velocityY = core::JUMP_SPEED;
                m_player->setAnimation("jump");
            }
        }

        if (m_input.isKeyPressed(core::KeyCode::Z)) {
            if (m_player->m_punchCooldown <= 0.0f) {
                m_player->setAnimation("punch");
                m_player->m_punchCooldown = Player::PUNCH_COOLDOWN;

                if (m_player->isCrouching()) {
                    // Crouched punch: hits capivara in front within range
                    static constexpr float PUNCH_RANGE_X = 70.0f;
                    static constexpr float PUNCH_RANGE_Y = 100.0f;
                    bool hitCapivara = false;
                    auto pPos = m_player->getPosition();
                    auto pDir = m_player->getDirection();
                    for (auto& c : m_capivaras) {
                        if (!c.isDead()) {
                            auto cPos = c.getPosition();
                            float dx = cPos.x - pPos.x;
                            float dy = std::abs(cPos.y - pPos.y);
                            bool inFront = (pDir == core::Direction::Right && dx > 0) ||
                                          (pDir == core::Direction::Left  && dx < 0);
                            if (inFront && std::abs(dx) < PUNCH_RANGE_X && dy < PUNCH_RANGE_Y) {
                                int dmg = m_damageCfg.getDamage(
                                    core::AttackType::Punch, core::EntityType::Capivara);
                                c.takeDamage(dmg);
                                hitCapivara = true;
                                break;
                            }
                        }
                    }
                    if (!hitCapivara && m_boss && !m_boss->health.isDead()) {
                        float dx = std::abs(m_boss->getPosition().x
                                            - m_player->getPosition().x);
                        if (dx <= PUNCH_RANGE_X) {
                            int dmg = m_damageCfg.getDamage(
                                core::AttackType::Punch, core::EntityType::Professor);
                            m_boss->health.takeDamage(dmg);
                        }
                    }
                } else {
                    // Standing punch: only hits boss within range
                    static constexpr float PUNCH_RANGE = 80.0f;
                    if (m_boss && !m_boss->health.isDead()) {
                        float dx = std::abs(m_boss->getPosition().x
                                            - m_player->getPosition().x);
                        if (dx <= PUNCH_RANGE) {
                            int dmg = m_damageCfg.getDamage(
                                core::AttackType::Punch, core::EntityType::Professor);
                            m_boss->health.takeDamage(dmg);
                        }
                    }
                }
            }
        }

        if (m_input.isKeyPressed(core::KeyCode::X)) {
            m_player->setAnimation("throw");
            auto& assets = infrastructure::AssetManager::instance();
            m_player->throwProjectile(m_projectiles, m_frameConfig,
                                      assets.getTexture("caneta"));
        }

        if (m_input.isKeyPressed(core::KeyCode::C)) {
            m_player->setAnimation("defend");
            m_player->defend(true);
        } else {
            m_player->defend(false);
        }

        if (!moving &&
            !m_input.isKeyPressed(core::KeyCode::Z) &&
            !m_input.isKeyPressed(core::KeyCode::X) &&
            !m_input.isKeyPressed(core::KeyCode::C) &&
            !m_input.isKeyPressed(core::KeyCode::Space) &&
            !m_input.isKeyPressed(core::KeyCode::Up) &&
            !m_player->isAttacking()) {  // let punch/throw finish
            m_player->setAnimation("idle");
        }
    }
}

// ────────────────────────────────────────────────────────────────────
// Menu input
// ────────────────────────────────────────────────────────────────────

void Game::handleMenuInput(const core::Event& event) {
    const int n = static_cast<int>(m_menuItems.size());

    if (event.key == core::KeyCode::Up) {
        m_menuSelection = (m_menuSelection - 1 + n) % n;
        updateMenuTextColors();
    } else if (event.key == core::KeyCode::Down) {
        m_menuSelection = (m_menuSelection + 1) % n;
        updateMenuTextColors();
    } else if (event.key == core::KeyCode::Enter) {
        switch (m_menuSelection) {
        case 0: // Start
            m_currentPhase = 0;
            loadLevel(m_currentPhase);
            setState(State::Playing);
            break;
        case 1: // Options
            m_optionsFromPause = false;
            m_menuPage = MenuPage::Options;
            m_optionsSelection = 0;
            break;
        case 2: // Info
            m_menuPage = MenuPage::Info;
            m_infoSelection = 0;
            break;
        case 3: // Restart
            restartGame();
            break;
        }
    }
}

void Game::handleInfoInput(const core::Event& event) {
    if (event.key == core::KeyCode::Up) {
        m_infoSelection = 0;  // only one item, stays at Back
    } else if (event.key == core::KeyCode::Down) {
        m_infoSelection = 0;
    } else if (event.key == core::KeyCode::Enter) {
        m_menuPage = MenuPage::Main;
    }
    // Escape is handled globally in processInput
}

void Game::handleOptionsInput(const core::Event& event) {
    const int n = 4;  // Music, SFX, Track, Back

    if (event.key == core::KeyCode::Up) {
        m_optionsSelection = (m_optionsSelection - 1 + n) % n;
    } else if (event.key == core::KeyCode::Down) {
        m_optionsSelection = (m_optionsSelection + 1) % n;
    } else if (event.key == core::KeyCode::Left) {
        if (m_optionsSelection == 0) {
            m_audioCfg.musicVolume = std::max(0.0f, m_audioCfg.musicVolume - 5.0f);
            m_audio->setMusicVolume(m_audioCfg.musicVolume);
        } else if (m_optionsSelection == 1) {
            m_audioCfg.effectVolume = std::max(0.0f, m_audioCfg.effectVolume - 5.0f);
        } else if (m_optionsSelection == 2) {
            m_audio->previousTrack();
            m_audioCfg.selectedTrack = m_audio->getCurrentTrackIndex();
        }
    } else if (event.key == core::KeyCode::Right) {
        if (m_optionsSelection == 0) {
            m_audioCfg.musicVolume = std::min(100.0f, m_audioCfg.musicVolume + 5.0f);
            m_audio->setMusicVolume(m_audioCfg.musicVolume);
        } else if (m_optionsSelection == 1) {
            m_audioCfg.effectVolume = std::min(100.0f, m_audioCfg.effectVolume + 5.0f);
        } else if (m_optionsSelection == 2) {
            m_audio->nextTrack();
            m_audioCfg.selectedTrack = m_audio->getCurrentTrackIndex();
        }
    } else if (event.key == core::KeyCode::Enter) {
        if (m_optionsSelection == 3) {
            if (m_optionsFromPause) {
                m_optionsFromPause = false;
                m_state = State::Paused;
            } else {
                m_menuPage = MenuPage::Main;
            }
        }
    }
    // Escape handled globally → returns to Main (or Pause if m_optionsFromPause)
}

void Game::handlePauseInput(const core::Event& event) {
    const int n = static_cast<int>(m_pauseItems.size());

    if (event.key == core::KeyCode::Up) {
        m_pauseSelection = (m_pauseSelection - 1 + n) % n;
    } else if (event.key == core::KeyCode::Down) {
        m_pauseSelection = (m_pauseSelection + 1) % n;
    } else if (event.key == core::KeyCode::Enter) {
        switch (m_pauseSelection) {
        case 0: // Resume
            setState(State::Playing);
            break;
        case 1: // Options
            m_optionsFromPause = true;
            m_optionsSelection = 0;
            m_menuPage = MenuPage::Options;
            break;
        case 2: // Restart (phase)
            restartPhase();
            setState(State::Playing);
            break;
        case 3: // Quit to Menu
            restartGame();
            setState(State::Menu);
            break;
        }
    }
}

void Game::handleGameOverInput(const core::Event& event) {
    if (event.key == core::KeyCode::Enter) {
        restartGame();
        setState(State::Menu);
    }
}

void Game::updateMenuTextColors() {
    for (int i = 0; i < static_cast<int>(m_menuItems.size()); ++i) {
        m_menuItems[i].setFillColor(
            (i == m_menuSelection) ? sf::Color::Yellow : sf::Color::White);
    }
}

// ────────────────────────────────────────────────────────────────────
void Game::update(float dt) {
    if (m_state != State::Playing) return;

    if (!m_player) return;

    // ── Check game over (lives depleted) ─────────────────────────
    if (m_player->lives.isGameOver() && m_player->health.isDead()) {
        const auto& phase = m_phaseConfig.getPhase(m_currentPhase);
        if (phase.hasBoss) {
            m_gameOverTitle.setString("JUBILADO");
        } else {
            m_gameOverTitle.setString("REPROVADO");
        }
        m_gameOverOverlay.setFillColor(sf::Color(80, 10, 10, 235));
        m_gameOverTitle.setFillColor(sf::Color::Red);
        setState(State::GameOver);
        return;
    }

    auto& assets = infrastructure::AssetManager::instance();

    // ── Capivara AI ──────────────────────────────────────────────
    for (auto& c : m_capivaras) {
        if (!c.isDead() && m_player) {
            c.update(dt, *m_player);
        }
    }

    // ── Boss AI ──────────────────────────────────────────────────
    if (m_boss && m_player) {
        m_boss->update(dt, *m_player, m_projectiles, m_frameConfig);
    }

    // ── Chest interaction ─────────────────────────────────────────
    if (m_chest && m_player && !m_chest->isOpen()) {
        m_chest->update(*m_player);
    }

    // ── Projectiles update ───────────────────────────────────────
    for (auto& p : m_projectiles) {
        if (p->isActive()) {
            p->update(dt);
        }
    }

    // ── Projectile collisions ────────────────────────────────────
    for (auto& p : m_projectiles) {
        if (!p->isActive()) continue;

        auto bounds = p->getBounds();

        // Pen (player projectile) vs enemies
        if (p->getDamage() == 20) {  // Pen
            for (auto& c : m_capivaras) {
                if (c.isDead()) continue;
                auto cPos = c.getPosition();
                sf::FloatRect cBounds(cPos.x, cPos.y, 60.0f, 80.0f);
                if (bounds.intersects(cBounds)) {
                    c.takeDamage(p->getDamage());
                    p->deactivate();
                    break;
                }
            }
            if (p->isActive() && m_boss && !m_boss->health.isDead()) {
                auto profPos = m_boss->getPosition();
                sf::FloatRect pBounds(profPos.x, profPos.y, 80.0f, 80.0f);
                if (bounds.intersects(pBounds)) {
                    m_boss->health.takeDamage(p->getDamage());
                    p->deactivate();
                }
            }
        } else {  // Boss projectile vs player
            // Grace period: skip collision for first 0.1s so projectile
            // is visible before it can be destroyed (lifetime starts at 3.0)
            if (p->getLifetime() > 2.9f) continue;

            if (!m_player->health.isDead()) {
                auto pPos = m_player->getPosition();
                float playerH = m_player->getCurrentHeight();
                sf::FloatRect pBounds(pPos.x, pPos.y, 60.0f, playerH);
                if (bounds.intersects(pBounds)) {
                    m_player->takeHit(core::AttackType::BossProjectile, m_damageCfg);
                    p->deactivate();
                }
            }
        }
    }

    // ── Cleanup inactive projectiles ─────────────────────────────
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
                       [](const auto& p) { return !p->isActive(); }),
        m_projectiles.end());

    // ── Player ───────────────────────────────────────────────────
    if (!m_player->health.isDead()) {
        m_player->updateAnimation(dt);
        m_player->applyGravity(dt);

        auto pos = m_player->getPosition();
        m_player->setPosition(
            {std::clamp(pos.x, 0.0f, 1860.0f), pos.y});

        if (m_playerHealthBar) {
            auto p = m_player->getPosition();
            m_playerHealthBar->setPosition({p.x - 50.0f, p.y - 30.0f});
        }

        for (auto& c : m_capivaras) {
            if (c.isDead()) continue;
            auto cPos = c.getPosition();
            auto pPos = m_player->getPosition();
            float dx = std::abs(cPos.x - pPos.x);
            float dy = std::abs(cPos.y - pPos.y);
            if (dx < Capivara::ATTACK_RANGE && dy < 80.0f) {
                c.touchPlayer(*m_player, m_damageCfg);
            }
        }

        if (m_player->health.isDead() && !m_player->lives.isGameOver()) {
            m_player->revive();
        }
    }

    std::size_t barIdx = 0;
    for (auto& c : m_capivaras) {
        if (barIdx < m_enemyHealthBars.size()) {
            auto pos = c.getPosition();
            m_enemyHealthBars[barIdx].setPosition({pos.x - 50.0f, pos.y - 30.0f});
        }
        ++barIdx;
    }
    if (m_boss && barIdx < m_enemyHealthBars.size()) {
        auto pos = m_boss->getPosition();
        m_enemyHealthBars[barIdx].setPosition({pos.x - 50.0f, pos.y - 30.0f});
    }

    // ── Check phase completion (all enemies dead) ────────────────
    bool allDead = true;
    for (auto& c : m_capivaras) {
        if (!c.isDead()) { allDead = false; break; }
    }
    if (m_boss && !m_boss->health.isDead()) {
        allDead = false;
    }

    if (allDead) {
        if (m_currentPhase + 1 >= m_phaseConfig.size()) {
            // All phases complete → Victory
            m_gameOverTitle.setString("Formado!");
            m_gameOverTitle.setFillColor(sf::Color(255, 215, 0));  // gold
            m_gameOverOverlay.setFillColor(sf::Color(10, 40, 10, 235));
            setState(State::Victory);
        } else {
            advancePhase();
        }
    }
}

// ────────────────────────────────────────────────────────────────────
void Game::render() {
    m_renderer.clear(core::Color{10, 10, 30});

    if (m_state == State::Menu) {
        renderMenu();

        if (m_menuPage == MenuPage::Options) {
            renderOptions();
        } else if (m_menuPage == MenuPage::Info) {
            renderInfo();
        }
    } else if (m_state == State::Playing || m_state == State::Paused) {
        // Draw game layer (frozen in pause)
        if (m_currentLevel) {
            m_currentLevel->draw(m_renderer);
        }
        for (auto& c : m_capivaras) {
            if (!c.isDead()) {
                m_renderer.draw(c);
            }
        }
        if (m_boss && !m_boss->health.isDead()) {
            m_renderer.draw(*m_boss);
        }
        if (m_chest) {
            m_renderer.draw(*m_chest);
        }
        if (m_player && !m_player->health.isDead()) {
            m_renderer.draw(*m_player);
        }

        // Projectiles
        for (auto& p : m_projectiles) {
            if (p->isActive()) {
                m_renderer.draw(*p);
            }
        }

        for (auto& bar : m_enemyHealthBars) {
            m_renderer.draw(bar);
        }
        if (m_playerHealthBar) {
            m_renderer.draw(*m_playerHealthBar);
        }
        if (m_livesDisplay) {
            m_renderer.draw(*m_livesDisplay);
        }
        if (m_ammoDisplay) {
            const auto wSz = m_renderer.getSize();
            m_ammoDisplay->setPosition({10.0f, 40.0f});
            m_renderer.draw(*m_ammoDisplay);
        }

        // Pause / Options-from-Pause overlay on top
        if (m_state == State::Paused) {
            if (m_menuPage == MenuPage::Options) {
                renderOptions();
            } else {
                renderPauseMenu();
            }
        }
    } else if (m_state == State::GameOver) {
        // Draw game layer frozen underneath
        if (m_currentLevel) {
            m_currentLevel->draw(m_renderer);
        }
        for (auto& c : m_capivaras) {
            if (!c.isDead()) m_renderer.draw(c);
        }
        if (m_boss && !m_boss->health.isDead()) m_renderer.draw(*m_boss);
        if (m_chest) m_renderer.draw(*m_chest);
        if (m_player && !m_player->health.isDead()) m_renderer.draw(*m_player);

        renderGameOver();
    } else if (m_state == State::Victory) {
        if (m_currentLevel) {
            m_currentLevel->draw(m_renderer);
        }
        renderVictory();
    }

    m_renderer.display();
}

// ────────────────────────────────────────────────────────────────────
// Menu rendering
// ────────────────────────────────────────────────────────────────────

void Game::renderMenu() {
    const auto wSz = m_renderer.getSize();
    const float cx = static_cast<float>(wSz.x) / 2.0f;

    // Background
    const auto* bgTex = m_menuBg.getSfmlSprite().getTexture();
    if (bgTex && bgTex->getSize().x > 0) {
        m_menuBg.setScale(
            static_cast<float>(wSz.x) / bgTex->getSize().x,
            static_cast<float>(wSz.y) / bgTex->getSize().y);
    }
    m_menuBg.setPosition(0.0f, 0.0f);
    m_renderer.draw(m_menuBg);

    // Title
    const auto bounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin(bounds.left + bounds.width  / 2.0f,
                          bounds.top  + bounds.height / 2.0f);
    m_titleText.setPosition(cx, 150.0f);
    m_renderer.draw(m_titleText);

    auto& sfml = dynamic_cast<infrastructure::SfmlRenderer&>(m_renderer);

    // Panel + items (hidden when sub-page is active)
    if (m_menuPage == MenuPage::Main) {
        m_menuOverlay.setOrigin(m_menuOverlay.getSize().x / 2.0f,
                                m_menuOverlay.getSize().y / 2.0f);
        m_menuOverlay.setPosition(cx, 380.0f);
        sfml.drawSfml(m_menuOverlay);

        for (int i = 0; i < static_cast<int>(m_menuItems.size()); ++i) {
            auto& item = m_menuItems[i];
            const auto ib = item.getLocalBounds();
            item.setOrigin(ib.left + ib.width / 2.0f, ib.top + ib.height / 2.0f);
            item.setPosition(cx, 320.0f + i * 50.0f);
            m_renderer.draw(item);
        }
    }

    // Hearts always visible
    for (auto& h : m_menuHearts) {
        m_renderer.draw(h);
    }
}

// ────────────────────────────────────────────────────────────────────
// Info screen
// ────────────────────────────────────────────────────────────────────

void Game::renderInfo() {
    const auto wSz = m_renderer.getSize();
    const float cx = static_cast<float>(wSz.x) / 2.0f;
    const float cy = static_cast<float>(wSz.y) / 2.0f;

    auto& sfml = dynamic_cast<infrastructure::SfmlRenderer&>(m_renderer);

    // Full overlay — hides menu items behind
    m_infoOverlay.setOrigin(m_infoOverlay.getSize().x / 2.0f,
                            m_infoOverlay.getSize().y / 2.0f);
    m_infoOverlay.setPosition(cx, cy);
    sfml.drawSfml(m_infoOverlay);

    // Title
    auto tb = m_infoTitle.getLocalBounds();
    m_infoTitle.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    m_infoTitle.setPosition(cx, cy - 260.0f);
    m_renderer.draw(m_infoTitle);

    // Developer
    auto db = m_infoDev.getLocalBounds();
    m_infoDev.setOrigin(db.left + db.width / 2.0f, db.top + db.height / 2.0f);
    m_infoDev.setPosition(cx, cy - 200.0f);
    m_renderer.draw(m_infoDev);

    // Copyright
    auto cb = m_infoCopyright.getLocalBounds();
    m_infoCopyright.setOrigin(cb.left + cb.width / 2.0f, cb.top + cb.height / 2.0f);
    m_infoCopyright.setPosition(cx, cy - 160.0f);
    m_renderer.draw(m_infoCopyright);

    // Controls (with double newlines for spacing)
    auto ctb = m_infoControls.getLocalBounds();
    m_infoControls.setOrigin(ctb.left + ctb.width / 2.0f, ctb.top + ctb.height / 2.0f);
    m_infoControls.setPosition(cx, cy + 10.0f);
    m_renderer.draw(m_infoControls);

    // Back (selectable, highlighted when selected)
    m_infoBack.setFillColor(
        (m_infoSelection == 0) ? sf::Color::Yellow : sf::Color::White);
    auto bb = m_infoBack.getLocalBounds();
    m_infoBack.setOrigin(bb.left + bb.width / 2.0f, bb.top + bb.height / 2.0f);
    m_infoBack.setPosition(cx, cy + 260.0f);
    m_renderer.draw(m_infoBack);
}

// ────────────────────────────────────────────────────────────────────
// Pause menu
// ────────────────────────────────────────────────────────────────────

std::string Game::buildSliderBar(float value) const {
    constexpr int barLen = 16;
    int filled = static_cast<int>(value / 100.0f * barLen + 0.5f);
    std::string bar = "[";
    for (int i = 0; i < barLen; ++i) {
        bar += (i < filled) ? '=' : '-';
    }
    bar += "]  " + std::to_string(static_cast<int>(value)) + "%";
    return bar;
}

void Game::renderOptions() {
    const auto wSz = m_renderer.getSize();
    const float cx = static_cast<float>(wSz.x) / 2.0f;
    const float cy = static_cast<float>(wSz.y) / 2.0f;

    auto& sfml = dynamic_cast<infrastructure::SfmlRenderer&>(m_renderer);

    // Overlay
    m_optionsOverlay.setOrigin(m_optionsOverlay.getSize().x / 2.0f,
                               m_optionsOverlay.getSize().y / 2.0f);
    m_optionsOverlay.setPosition(cx, cy);
    sfml.drawSfml(m_optionsOverlay);

    // Title
    auto tb = m_optionsTitle.getLocalBounds();
    m_optionsTitle.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    m_optionsTitle.setPosition(cx, cy - 185.0f);
    m_renderer.draw(m_optionsTitle);

    // Music Volume — label above bar
    m_optionsMusicLabel.setFillColor(
        m_optionsSelection == 0 ? sf::Color::Yellow : sf::Color(200, 200, 255));
    auto mlb = m_optionsMusicLabel.getLocalBounds();
    m_optionsMusicLabel.setOrigin(mlb.left + mlb.width / 2.0f, mlb.top + mlb.height / 2.0f);
    m_optionsMusicLabel.setPosition(cx, cy - 120.0f);
    m_renderer.draw(m_optionsMusicLabel);

    m_optionsMusicBar.setString(buildSliderBar(m_audioCfg.musicVolume));
    m_optionsMusicBar.setFillColor(
        m_optionsSelection == 0 ? sf::Color::Yellow : sf::Color::White);
    auto mbb = m_optionsMusicBar.getLocalBounds();
    m_optionsMusicBar.setOrigin(mbb.left + mbb.width / 2.0f, mbb.top + mbb.height / 2.0f);
    m_optionsMusicBar.setPosition(cx, cy - 85.0f);
    m_renderer.draw(m_optionsMusicBar);

    // Sound Effects — label above bar
    m_optionsSfxLabel.setFillColor(
        m_optionsSelection == 1 ? sf::Color::Yellow : sf::Color(200, 200, 255));
    auto slb = m_optionsSfxLabel.getLocalBounds();
    m_optionsSfxLabel.setOrigin(slb.left + slb.width / 2.0f, slb.top + slb.height / 2.0f);
    m_optionsSfxLabel.setPosition(cx, cy - 30.0f);
    m_renderer.draw(m_optionsSfxLabel);

    m_optionsSfxBar.setString(buildSliderBar(m_audioCfg.effectVolume));
    m_optionsSfxBar.setFillColor(
        m_optionsSelection == 1 ? sf::Color::Yellow : sf::Color::White);
    auto sbb = m_optionsSfxBar.getLocalBounds();
    m_optionsSfxBar.setOrigin(sbb.left + sbb.width / 2.0f, sbb.top + sbb.height / 2.0f);
    m_optionsSfxBar.setPosition(cx, cy + 5.0f);
    m_renderer.draw(m_optionsSfxBar);

    // Background Music — label above selector
    m_optionsTrackLabel.setFillColor(
        m_optionsSelection == 2 ? sf::Color::Yellow : sf::Color(200, 200, 255));
    auto tlb = m_optionsTrackLabel.getLocalBounds();
    m_optionsTrackLabel.setOrigin(tlb.left + tlb.width / 2.0f, tlb.top + tlb.height / 2.0f);
    m_optionsTrackLabel.setPosition(cx, cy + 55.0f);
    m_renderer.draw(m_optionsTrackLabel);

    m_optionsTrackSelector.setString(
        "<  " + m_audio->getCurrentTrackLabel() + "  >");
    m_optionsTrackSelector.setFillColor(
        m_optionsSelection == 2 ? sf::Color::Yellow : sf::Color::White);
    auto tsb = m_optionsTrackSelector.getLocalBounds();
    m_optionsTrackSelector.setOrigin(tsb.left + tsb.width / 2.0f, tsb.top + tsb.height / 2.0f);
    m_optionsTrackSelector.setPosition(cx, cy + 85.0f);
    m_renderer.draw(m_optionsTrackSelector);

    // Hint
    auto hb = m_optionsHint.getLocalBounds();
    m_optionsHint.setOrigin(hb.left + hb.width / 2.0f, hb.top + hb.height / 2.0f);
    m_optionsHint.setPosition(cx, cy + 130.0f);
    m_renderer.draw(m_optionsHint);

    // Back
    m_optionsBack.setFillColor(
        m_optionsSelection == 3 ? sf::Color::Yellow : sf::Color::White);
    auto bb = m_optionsBack.getLocalBounds();
    m_optionsBack.setOrigin(bb.left + bb.width / 2.0f, bb.top + bb.height / 2.0f);
    m_optionsBack.setPosition(cx, cy + 180.0f);
    m_renderer.draw(m_optionsBack);
}

void Game::renderPauseMenu() {
    const auto wSz = m_renderer.getSize();
    const float cx = static_cast<float>(wSz.x) / 2.0f;
    const float cy = static_cast<float>(wSz.y) / 2.0f;

    auto& sfml = dynamic_cast<infrastructure::SfmlRenderer&>(m_renderer);

    // Dim overlay
    sf::RectangleShape dim;
    dim.setSize({static_cast<float>(wSz.x), static_cast<float>(wSz.y)});
    dim.setFillColor(sf::Color(0, 0, 0, 120));
    sfml.drawSfml(dim);

    // Panel
    m_pauseOverlay.setOrigin(m_pauseOverlay.getSize().x / 2.0f,
                             m_pauseOverlay.getSize().y / 2.0f);
    m_pauseOverlay.setPosition(cx, cy);
    sfml.drawSfml(m_pauseOverlay);

    // Options
    for (int i = 0; i < static_cast<int>(m_pauseItems.size()); ++i) {
        auto& item = m_pauseItems[i];
        item.setFillColor(
            (i == m_pauseSelection) ? sf::Color::Yellow : sf::Color::White);
        const auto ib = item.getLocalBounds();
        item.setOrigin(ib.left + ib.width / 2.0f, ib.top + ib.height / 2.0f);
        item.setPosition(cx, cy - 70.0f + i * 55.0f);
        m_renderer.draw(item);
    }
}

// ────────────────────────────────────────────────────────────────────
// Game Over / Victory rendering
// ────────────────────────────────────────────────────────────────────

void Game::renderGameOver() {
    const auto wSz = m_renderer.getSize();
    const float cx = static_cast<float>(wSz.x) / 2.0f;
    const float cy = static_cast<float>(wSz.y) / 2.0f;

    auto& sfml = dynamic_cast<infrastructure::SfmlRenderer&>(m_renderer);

    // Red-tinted fullscreen dim
    sf::RectangleShape dim;
    dim.setSize({static_cast<float>(wSz.x), static_cast<float>(wSz.y)});
    dim.setFillColor(sf::Color(120, 0, 0, 150));
    sfml.drawSfml(dim);

    // Panel
    m_gameOverOverlay.setOrigin(m_gameOverOverlay.getSize().x / 2.0f,
                                m_gameOverOverlay.getSize().y / 2.0f);
    m_gameOverOverlay.setPosition(cx, cy);
    sfml.drawSfml(m_gameOverOverlay);

    // Title (REPROVADO / JUBILADO)
    auto tb = m_gameOverTitle.getLocalBounds();
    m_gameOverTitle.setOrigin(tb.left + tb.width / 2.0f,
                              tb.top + tb.height / 2.0f);
    m_gameOverTitle.setPosition(cx, cy - 40.0f);
    m_renderer.draw(m_gameOverTitle);

    // Subtext
    auto sb = m_gameOverSubtext.getLocalBounds();
    m_gameOverSubtext.setOrigin(sb.left + sb.width / 2.0f,
                                sb.top + sb.height / 2.0f);
    m_gameOverSubtext.setPosition(cx, cy + 70.0f);
    m_renderer.draw(m_gameOverSubtext);
}

void Game::renderVictory() {
    const auto wSz = m_renderer.getSize();
    const float cx = static_cast<float>(wSz.x) / 2.0f;
    const float cy = static_cast<float>(wSz.y) / 2.0f;

    auto& sfml = dynamic_cast<infrastructure::SfmlRenderer&>(m_renderer);

    // Green-tinted fullscreen dim
    sf::RectangleShape dim;
    dim.setSize({static_cast<float>(wSz.x), static_cast<float>(wSz.y)});
    dim.setFillColor(sf::Color(0, 80, 0, 140));
    sfml.drawSfml(dim);

    // Panel
    m_gameOverOverlay.setOrigin(m_gameOverOverlay.getSize().x / 2.0f,
                                m_gameOverOverlay.getSize().y / 2.0f);
    m_gameOverOverlay.setPosition(cx, cy);
    sfml.drawSfml(m_gameOverOverlay);

    // Title
    auto tb = m_gameOverTitle.getLocalBounds();
    m_gameOverTitle.setOrigin(tb.left + tb.width / 2.0f,
                              tb.top + tb.height / 2.0f);
    m_gameOverTitle.setPosition(cx, cy - 40.0f);
    m_renderer.draw(m_gameOverTitle);

    // Subtext
    auto sb = m_gameOverSubtext.getLocalBounds();
    m_gameOverSubtext.setOrigin(sb.left + sb.width / 2.0f,
                                sb.top + sb.height / 2.0f);
    m_gameOverSubtext.setPosition(cx, cy + 70.0f);
    m_renderer.draw(m_gameOverSubtext);
}

// ────────────────────────────────────────────────────────────────────
void Game::setState(State newState) {
    m_state = newState;
}

void Game::restartGame() {
    m_capivaras.clear();
    m_enemyHealthBars.clear();
    m_boss.reset();
    m_player.reset();
    m_currentLevel.reset();
    m_livesDisplay.reset();
    m_ammoDisplay.reset();
    m_playerHealthBar.reset();
    m_projectiles.clear();

    m_currentPhase = 0;
    loadLevel(m_currentPhase);
}

void Game::restartPhase() {
    m_capivaras.clear();
    m_enemyHealthBars.clear();
    m_boss.reset();
    m_player.reset();
    m_currentLevel.reset();
    m_livesDisplay.reset();
    m_ammoDisplay.reset();
    m_playerHealthBar.reset();
    m_projectiles.clear();

    loadLevel(m_currentPhase);
}

void Game::advancePhase() {
    m_currentPhase++;
    m_capivaras.clear();
    m_enemyHealthBars.clear();
    m_boss.reset();
    m_currentLevel.reset();
    m_playerHealthBar.reset();
    m_projectiles.clear();

    loadLevel(m_currentPhase);
}

void Game::loadLevel(int phaseIndex) {
    m_currentPhase = phaseIndex;
    const auto& phase = m_phaseConfig.getPhase(phaseIndex);
    float groundY = phase.groundY;

    // Create Level with the configured background path and ground level
    m_currentLevel = std::make_unique<Level>(phase.background, groundY);

    auto& assets = infrastructure::AssetManager::instance();

    assets.loadTexture("player",
                       "assets/sprites/player/player_sheet.png");
    assets.loadTexture("capivara",
                       "assets/sprites/capivara/capivara_sheet.png");
    assets.loadTexture("professor",
                       "assets/sprites/professor/professor_sheet.png");
    assets.loadTexture("rato",
                       "assets/sprites/rato/rato_sheet.png");
    assets.loadTexture("mandrake",
                       "assets/sprites/mandrake/mandrake_sheet.png");
    assets.loadTexture("peru",
                       "assets/sprites/peru/peru_sheet.png");
    assets.loadTexture("exam",
                       "assets/projectiles/livro.png");
    assets.loadTexture("panela",
                       "assets/projectiles/panela.png");
    assets.loadTexture("copo",
                       "assets/projectiles/copo.png");
    assets.loadTexture("pedra",
                       "assets/projectiles/pedra.png");

    // ── Player ───────────────────────────────────────────────────
    // Preserve lives/ammo if player already exists (phase transition)
    int savedLives = 5;
    int savedAmmo  = 10;
    if (m_player) {
        savedLives = m_player->lives.currentLives;
        savedAmmo  = m_player->ammo.currentAmmo;
    }
    m_player = std::make_unique<Player>(
        assets.getTexture("player"), m_frameConfig);
    m_player->setGroundY(groundY);
    m_player->lives.currentLives = savedLives;
    m_player->ammo.currentAmmo  = savedAmmo;

    m_livesDisplay = std::make_unique<infrastructure::LivesDisplay>(
        m_player->lives, assets.getTexture("heart"));
    m_livesDisplay->setPosition({10.0f, 10.0f});

    m_ammoDisplay = std::make_unique<infrastructure::AmmoDisplay>(
        m_player->ammo, assets.getTexture("caneta_disp"), 0.30f, 36.0f);

    m_playerHealthBar = std::make_unique<infrastructure::HealthBar>(
        m_player->health);
    auto pPos = m_player->getPosition();
    m_playerHealthBar->setPosition({pPos.x - 50.0f, pPos.y - 30.0f});

    // ── Enemies (from config) ────────────────────────────────────
    m_capivaras.clear();
    m_enemyHealthBars.clear();
    m_boss.reset();

    int enemyCount = phase.enemyCount;
    m_capivaras.reserve(static_cast<std::size_t>(enemyCount));
    m_enemyHealthBars.reserve(
        static_cast<std::size_t>(enemyCount + (phase.hasBoss ? 1 : 0)));
    for (int i = 0; i < enemyCount; ++i) {
        float startX = 1500.0f + i * 200.0f;
        auto& c = m_capivaras.emplace_back(
            assets.getTexture("capivara"), m_frameConfig, startX);
        c.setGroundY(groundY);
        auto& bar = m_enemyHealthBars.emplace_back(c.health);
        auto cPos = c.getPosition();
        bar.setPosition({cPos.x - 50.0f, cPos.y - 30.0f});
    }

    if (phase.hasBoss) {
        const auto& bossType = phase.bossType;
        if (bossType == "rato") {
            m_boss = std::make_unique<Rato>(
                assets.getTexture("rato"), m_frameConfig);
            m_boss->setProjectileTexture(assets.getTexture("panela"));
        } else if (bossType == "mandrake") {
            m_boss = std::make_unique<Mandrake>(
                assets.getTexture("mandrake"), m_frameConfig);
            m_boss->setProjectileTexture(assets.getTexture("pedra"));
        } else if (bossType == "peru") {
            m_boss = std::make_unique<Peru>(
                assets.getTexture("peru"), m_frameConfig);
            m_boss->setProjectileTexture(assets.getTexture("copo"));
        } else {
            m_boss = std::make_unique<Professor>(
                assets.getTexture("professor"), m_frameConfig);
            m_boss->setProjectileTexture(assets.getTexture("exam"));
        }
        m_boss->setGroundY(groundY);
        m_boss->setPosition({1700.0f, groundY - m_boss->getHeight()});
        auto& bar = m_enemyHealthBars.emplace_back(m_boss->health);
        auto bossPos = m_boss->getPosition();
        bar.setPosition({bossPos.x - 50.0f, bossPos.y - 30.0f});
    }

    // ── Chest (Phase 5 only) ──────────────────────────────────────
    m_chest.reset();
    if (m_currentPhase == 4) {  // 0-based: phase 5 = index 4 (Sanfran)
        assets.loadTexture("chest",
                           "assets/sprites/chest/chest_sheet.png");
        m_chest = std::make_unique<Chest>();
        m_chest->init(assets.getTexture("chest"), m_frameConfig,
                      {200.0f, groundY - Chest::CHEST_HEIGHT}, groundY);
    }
}

} // namespace gameplay
