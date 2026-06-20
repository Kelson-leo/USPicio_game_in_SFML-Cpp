#include "Game.h"
#include "Level.h"
#include "infrastructure/AssetManager.h"
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

    // ── Menu background (stretch to fill window) ─────────────────
    m_menuBg.setTexture(assets.getTexture("background_fase1"), true);

    // ── Title text ───────────────────────────────────────────────
    m_titleText.setFont(m_font);
    m_titleText.setString("USPICIO");
    m_titleText.setCharacterSize(48);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);
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
            if (event.key == core::KeyCode::Escape) {
                m_running = false;
                m_renderer.close();
                return;
            }

            if (m_state == State::Menu) {
                if (event.key == core::KeyCode::Enter) {
                    loadLevel(1);
                    setState(State::Playing);
                }
            }
        }
    }

    // ── Real-time input (Playing state) ──────────────────────────
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
            // Punch nearest capivara
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
            !m_input.isKeyPressed(core::KeyCode::Up)) {
            m_player->setAnimation("idle");
        }
    }
}

// ────────────────────────────────────────────────────────────────────
void Game::update(float dt) {
    if (m_state != State::Playing) return;

    if (m_player && !m_player->health.isDead()) {
        m_player->updateAnimation(dt);
        m_player->applyGravity(dt);

        // Update health bar position
        if (m_playerHealthBar) {
            auto p = m_player->getPosition();
            m_playerHealthBar->setPosition({p.x - 50.0f, p.y - 30.0f});
        }

        // Enemy touch damage (simple proximity)
        for (auto& c : m_capivaras) {
            if (c.health.isDead()) continue;
            auto cPos = c.getPosition();
            auto pPos = m_player->getPosition();
            float dx = std::abs(cPos.x - pPos.x);
            float dy = std::abs(cPos.y - pPos.y);
            if (dx < 60.0f && dy < 60.0f) {
                c.touchPlayer(*m_player, m_damageCfg);
            }
        }

        // Check player death + revive
        if (m_player->health.isDead() && !m_player->lives.isGameOver()) {
            m_player->revive();
        }
    }

    // Update enemy health bar positions
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
        const auto wSz = m_renderer.getSize();
        const auto* bgTex = m_menuBg.getSfmlSprite().getTexture();
        if (bgTex && bgTex->getSize().x > 0) {
            m_menuBg.setScale(
                static_cast<float>(wSz.x) / bgTex->getSize().x,
                static_cast<float>(wSz.y) / bgTex->getSize().y);
        }
        m_menuBg.setPosition(0.0f, 0.0f);
        m_renderer.draw(m_menuBg);

        const auto bounds = m_titleText.getLocalBounds();
        m_titleText.setOrigin(bounds.left + bounds.width  / 2.0f,
                              bounds.top  + bounds.height / 2.0f);
        m_titleText.setPosition(wSz.x / 2.0f, wSz.y / 2.0f);
        m_renderer.draw(m_titleText);
    } else if (m_state == State::Playing) {
        if (m_currentLevel) {
            m_currentLevel->draw(m_renderer);
        }

        // ── Enemies (behind player) ──────────────────────────────
        for (auto& c : m_capivaras) {
            if (!c.health.isDead()) {
                m_renderer.draw(c);
            }
        }
        if (m_professor && !m_professor->health.isDead()) {
            m_renderer.draw(*m_professor);
        }

        // ── Player ───────────────────────────────────────────────
        if (m_player && !m_player->health.isDead()) {
            m_renderer.draw(*m_player);
        }

        // ── Enemy health bars ────────────────────────────────────
        for (auto& bar : m_enemyHealthBars) {
            m_renderer.draw(bar);
        }

        // ── Player health bar ────────────────────────────────────
        if (m_playerHealthBar) {
            m_renderer.draw(*m_playerHealthBar);
        }

        // ── HUD ──────────────────────────────────────────────────
        if (m_livesDisplay) {
            m_renderer.draw(*m_livesDisplay);
        }

        if (m_ammoDisplay) {
            const auto wSz = m_renderer.getSize();
            m_ammoDisplay->setPosition(
                {static_cast<float>(wSz.x) - 320.0f, 10.0f});
            m_renderer.draw(*m_ammoDisplay);
        }
    }

    m_renderer.display();
}

// ────────────────────────────────────────────────────────────────────
void Game::setState(State newState) {
    m_state = newState;
}

void Game::loadLevel(int phaseNumber) {
    m_currentLevel = std::make_unique<Level>(phaseNumber);

    auto& assets = infrastructure::AssetManager::instance();

    // ── Load character textures ──────────────────────────────────
    assets.loadTexture("player",
                       "assets/sprites/player/player_sheet.png");
    assets.loadTexture("capivara",
                       "assets/sprites/capivara/capivara_sheet.png");
    assets.loadTexture("professor",
                       "assets/sprites/professor/professor_sheet.png");

    // ── Player ───────────────────────────────────────────────────
    m_player = std::make_unique<Player>(
        assets.getTexture("player"), m_frameConfig);

    // Lives display (top-left HUD)
    m_livesDisplay = std::make_unique<infrastructure::LivesDisplay>(
        m_player->lives, assets.getTexture("heart"));
    m_livesDisplay->setPosition({10.0f, 10.0f});

    // Ammo display (top-right HUD)
    m_ammoDisplay = std::make_unique<infrastructure::AmmoDisplay>(
        m_player->ammo, assets.getTexture("caneta"));

    // Player health bar (above character)
    m_playerHealthBar = std::make_unique<infrastructure::HealthBar>(
        m_player->health);
    auto pPos = m_player->getPosition();
    m_playerHealthBar->setPosition({pPos.x - 50.0f, pPos.y - 30.0f});

    // ── Enemies ──────────────────────────────────────────────────
    m_capivaras.clear();
    m_enemyHealthBars.clear();
    m_professor.reset();

    if (phaseNumber == 1) {
        // Phase 1: 3 capivaras
        for (int i = 0; i < 3; ++i) {
            auto& c = m_capivaras.emplace_back(
                assets.getTexture("capivara"), m_frameConfig);
            c.setPosition({300.0f + i * 150.0f, core::GROUND_Y});
            auto& bar = m_enemyHealthBars.emplace_back(c.health);
            auto cPos = c.getPosition();
            bar.setPosition({cPos.x - 50.0f, cPos.y - 30.0f});
        }
    } else if (phaseNumber == 2) {
        // Phase 2: 4 capivaras
        for (int i = 0; i < 4; ++i) {
            auto& c = m_capivaras.emplace_back(
                assets.getTexture("capivara"), m_frameConfig);
            c.setPosition({250.0f + i * 150.0f, core::GROUND_Y});
            auto& bar = m_enemyHealthBars.emplace_back(c.health);
            auto cPos = c.getPosition();
            bar.setPosition({cPos.x - 50.0f, cPos.y - 30.0f});
        }
    } else if (phaseNumber == 3) {
        // Phase 3: 2 capivaras + professor
        for (int i = 0; i < 2; ++i) {
            auto& c = m_capivaras.emplace_back(
                assets.getTexture("capivara"), m_frameConfig);
            c.setPosition({250.0f + i * 150.0f, core::GROUND_Y});
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
