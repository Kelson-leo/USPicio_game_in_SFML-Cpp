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

    // ── Preload menu / common assets ─────────────────────────────
    auto& assets = infrastructure::AssetManager::instance();
    assets.loadTexture("background_fase1",
                       "assets/backgrounds/fase1_patio.png");
    assets.loadTexture("heart",
                       "assets/ui/heart.png");
    assets.loadTexture("caneta",
                       "assets/projectiles/caneta.png");

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

    // ── Menu items ───────────────────────────────────────────────
    const char* labels[] = {"Start", "Restart", "Info"};
    for (const auto* lbl : labels) {
        auto& item = m_menuItems.emplace_back();
        item.setFont(m_font);
        item.setString(lbl);
        item.setCharacterSize(36);
        item.setFillColor(sf::Color::White);
    }

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
        "  Enter       -  Select / Start\n\n"
        "  Escape      -  Pause / Back");
    m_infoControls.setCharacterSize(18);
    m_infoControls.setFillColor(sf::Color(220, 220, 220));

    m_infoBack.setFont(m_font);
    m_infoBack.setString("Back (Esc / Enter)");
    m_infoBack.setCharacterSize(28);
    m_infoBack.setFillColor(sf::Color::Yellow);

    // ── Pause overlay ────────────────────────────────────────────
    m_pauseOverlay.setSize({380.0f, 220.0f});
    m_pauseOverlay.setFillColor(sf::Color(20, 20, 40, 230));

    const char* pauseOpts[] = {"Resume", "Restart", "Quit to Menu"};
    for (const auto* lbl : pauseOpts) {
        auto& item = m_pauseItems.emplace_back();
        item.setFont(m_font);
        item.setString(lbl);
        item.setCharacterSize(30);
        item.setFillColor(sf::Color::White);
    }

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
                if (m_state == State::Paused) {
                    setState(State::Playing);  // resume
                    continue;
                }
                if (m_state == State::Menu && m_menuPage == MenuPage::Info) {
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
                } else if (m_menuPage == MenuPage::Info) {
                    handleInfoInput(event);
                }
            } else if (m_state == State::Paused) {
                handlePauseInput(event);
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

        if (m_input.isKeyPressed(core::KeyCode::Space) ||
            m_input.isKeyPressed(core::KeyCode::Up)) {
            if (m_player->isOnGround()) {
                m_player->velocityY = core::JUMP_SPEED;
                m_player->setAnimation("jump");
            }
        }

        if (m_input.isKeyPressed(core::KeyCode::Z)) {
            m_player->setAnimation("punch");
            for (auto& c : m_capivaras) {
                if (!c.health.isDead()) {
                    m_player->punch(c.health, core::EntityType::Capivara, m_damageCfg);
                    break;
                }
            }
            if (m_professor && !m_professor->health.isDead()) {
                m_player->punch(m_professor->health, core::EntityType::Professor, m_damageCfg);
            }
        }

        if (m_input.isKeyPressed(core::KeyCode::X)) {
            m_player->setAnimation("throw");
            for (auto& c : m_capivaras) {
                if (!c.health.isDead()) {
                    m_player->throwCaneta(c.health, core::EntityType::Capivara, m_damageCfg);
                    break;
                }
            }
            if (m_professor && !m_professor->health.isDead()) {
                m_player->throwCaneta(m_professor->health, core::EntityType::Professor, m_damageCfg);
            }
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
            loadLevel(1);
            setState(State::Playing);
            break;
        case 1: // Restart
            restartGame();
            break;
        case 2: // Info
            m_menuPage = MenuPage::Info;
            m_infoSelection = 0;
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
        case 1: // Restart (phase)
            restartPhase();
            setState(State::Playing);
            break;
        case 2: // Quit to Menu
            restartGame();
            setState(State::Menu);
            break;
        }
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

    // ── Capivara AI ──────────────────────────────────────────────
    for (auto& c : m_capivaras) {
        if (!c.isDead() && m_player) {
            c.update(dt, *m_player);
        }
    }

    // ── Player ───────────────────────────────────────────────────
    if (m_player && !m_player->health.isDead()) {
        m_player->updateAnimation(dt);
        m_player->applyGravity(dt);

        // Clamp player within level boundaries
        auto pos = m_player->getPosition();
        m_player->setPosition(
            {std::clamp(pos.x, 0.0f, 1860.0f), pos.y});

        if (m_playerHealthBar) {
            auto p = m_player->getPosition();
            m_playerHealthBar->setPosition({p.x - 50.0f, p.y - 30.0f});
        }

        // Proximity damage from capivaras
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
    if (m_professor && barIdx < m_enemyHealthBars.size()) {
        auto pos = m_professor->getPosition();
        m_enemyHealthBars[barIdx].setPosition({pos.x - 50.0f, pos.y - 30.0f});
    }
}

// ────────────────────────────────────────────────────────────────────
void Game::render() {
    m_renderer.clear(core::Color{10, 10, 30});

    if (m_state == State::Menu) {
        renderMenu();

        if (m_menuPage == MenuPage::Info) {
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
        if (m_professor && !m_professor->health.isDead()) {
            m_renderer.draw(*m_professor);
        }
        if (m_player && !m_player->health.isDead()) {
            m_renderer.draw(*m_player);
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
            m_ammoDisplay->setPosition(
                {static_cast<float>(wSz.x) - 320.0f, 10.0f});
            m_renderer.draw(*m_ammoDisplay);
        }

        // Pause overlay on top
        if (m_state == State::Paused) {
            renderPauseMenu();
        }
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

    // Panel + items (hidden when Info page is active)
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
void Game::setState(State newState) {
    m_state = newState;
}

void Game::restartGame() {
    m_capivaras.clear();
    m_enemyHealthBars.clear();
    m_professor.reset();
    m_player.reset();
    m_currentLevel.reset();
    m_livesDisplay.reset();
    m_ammoDisplay.reset();
    m_playerHealthBar.reset();

    m_currentPhase = 1;
    loadLevel(1);
}

void Game::restartPhase() {
    m_capivaras.clear();
    m_enemyHealthBars.clear();
    m_professor.reset();
    m_player.reset();
    m_currentLevel.reset();
    m_livesDisplay.reset();
    m_ammoDisplay.reset();
    m_playerHealthBar.reset();

    loadLevel(m_currentPhase);
}

void Game::loadLevel(int phaseNumber) {
    m_currentPhase = phaseNumber;
    m_currentLevel = std::make_unique<Level>(phaseNumber);

    auto& assets = infrastructure::AssetManager::instance();

    assets.loadTexture("player",
                       "assets/sprites/player/player_sheet.png");
    assets.loadTexture("capivara",
                       "assets/sprites/capivara/capivara_sheet.png");
    assets.loadTexture("professor",
                       "assets/sprites/professor/professor_sheet.png");

    // ── Player ───────────────────────────────────────────────────
    m_player = std::make_unique<Player>(
        assets.getTexture("player"), m_frameConfig);

    m_livesDisplay = std::make_unique<infrastructure::LivesDisplay>(
        m_player->lives, assets.getTexture("heart"));
    m_livesDisplay->setPosition({10.0f, 10.0f});

    m_ammoDisplay = std::make_unique<infrastructure::AmmoDisplay>(
        m_player->ammo, assets.getTexture("caneta"));

    m_playerHealthBar = std::make_unique<infrastructure::HealthBar>(
        m_player->health);
    auto pPos = m_player->getPosition();
    m_playerHealthBar->setPosition({pPos.x - 50.0f, pPos.y - 30.0f});

    // ── Enemies ──────────────────────────────────────────────────
    m_capivaras.clear();
    m_enemyHealthBars.clear();
    m_professor.reset();

    if (phaseNumber == 1) {
        for (int i = 0; i < 2; ++i) {
            float startX = 1500.0f + i * 200.0f;
            auto& c = m_capivaras.emplace_back(
                assets.getTexture("capivara"), m_frameConfig, startX);
            auto& bar = m_enemyHealthBars.emplace_back(c.health);
            auto cPos = c.getPosition();
            bar.setPosition({cPos.x - 50.0f, cPos.y - 30.0f});
        }
    } else if (phaseNumber == 2) {
        for (int i = 0; i < 3; ++i) {
            float startX = 1500.0f + i * 200.0f;
            auto& c = m_capivaras.emplace_back(
                assets.getTexture("capivara"), m_frameConfig, startX);
            auto& bar = m_enemyHealthBars.emplace_back(c.health);
            auto cPos = c.getPosition();
            bar.setPosition({cPos.x - 50.0f, cPos.y - 30.0f});
        }
    } else if (phaseNumber == 3) {
        for (int i = 0; i < 2; ++i) {
            float startX = 1500.0f + i * 200.0f;
            auto& c = m_capivaras.emplace_back(
                assets.getTexture("capivara"), m_frameConfig, startX);
            auto& bar = m_enemyHealthBars.emplace_back(c.health);
            auto cPos = c.getPosition();
            bar.setPosition({cPos.x - 50.0f, cPos.y - 30.0f});
        }
        m_professor = std::make_unique<Professor>(
            assets.getTexture("professor"), m_frameConfig);
        m_professor->setPosition({700.0f, core::GROUND_Y});
        auto& bar = m_enemyHealthBars.emplace_back(m_professor->health);
        auto profPos = m_professor->getPosition();
        bar.setPosition({profPos.x - 50.0f, profPos.y - 30.0f});
    }
}

} // namespace gameplay
