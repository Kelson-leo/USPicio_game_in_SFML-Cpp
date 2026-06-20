#include "Game.h"
#include "Level.h"
#include "infrastructure/AssetManager.h"
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

    // ── Preload menu / common assets ─────────────────────────────
    auto& assets = infrastructure::AssetManager::instance();
    assets.loadTexture("background_fase1",
                       "assets/backgrounds/fase1_patio.png");
    assets.loadTexture("heart",
                       "assets/ui/heart.png");

    // ── Menu background (stretch to fill window) ─────────────────
    m_menuBg.setTexture(assets.getTexture("background_fase1"), true);

    // ── Title text ───────────────────────────────────────────────
    m_titleText.setFont(m_font);
    m_titleText.setString("USPICIO");
    m_titleText.setCharacterSize(48);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);

    // ── Heart sprites (5, top-left, visual only) ─────────────────
    const auto& heartTex = assets.getTexture("heart");
    for (int i = 0; i < 5; ++i) {
        auto& h = m_hearts.emplace_back(heartTex);
        h.setPosition(10.0f + i * 36.0f, 10.0f);
    }
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
}

// ────────────────────────────────────────────────────────────────────
void Game::update(float /*deltaTime*/) {
}

// ────────────────────────────────────────────────────────────────────
void Game::render() {
    m_renderer.clear(core::Color{10, 10, 30});

    if (m_state == State::Menu) {
        // ── Background ───────────────────────────────────────────
        const auto wSz = m_renderer.getSize();
        const auto* bgTex = m_menuBg.getSfmlSprite().getTexture();
        if (bgTex && bgTex->getSize().x > 0) {
            m_menuBg.setScale(
                static_cast<float>(wSz.x) / bgTex->getSize().x,
                static_cast<float>(wSz.y) / bgTex->getSize().y);
        }
        m_menuBg.setPosition(0.0f, 0.0f);
        m_renderer.draw(m_menuBg);

        // ── Title ────────────────────────────────────────────────
        const auto bounds = m_titleText.getLocalBounds();
        m_titleText.setOrigin(bounds.left + bounds.width  / 2.0f,
                              bounds.top  + bounds.height / 2.0f);
        m_titleText.setPosition(wSz.x / 2.0f, wSz.y / 2.0f);
        m_renderer.draw(m_titleText);

        // ── Hearts ───────────────────────────────────────────────
        for (auto& h : m_hearts) {
            m_renderer.draw(h);
        }
    } else if (m_state == State::Playing) {
        if (m_currentLevel) {
            m_currentLevel->draw(m_renderer);
        }
        // Hearts also visible during gameplay
        for (auto& h : m_hearts) {
            m_renderer.draw(h);
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
}

} // namespace gameplay
