#include "Game.h"
#include "infrastructure/SfmlRenderer.h"  // temporary: see render()
#include <iostream>

namespace gameplay {

// ────────────────────────────────────────────────────────────────────
Game::Game(core::IRenderer& renderer, core::IInputHandler& input)
    : m_renderer(renderer)
    , m_input(input) {
    // ── Load font from assets/ ────────────────────────────────────
    if (!m_font.loadFromFile("assets/fonts/PressStart2P.ttf")) {
        std::cerr << "[Game] WARNING: Could not load "
                     "assets/fonts/PressStart2P.ttf. "
                     "Text will not render.\n";
    }

    // ── Menu background ──────────────────────────────────────────
    m_menuBg.setSize({400.0f, 150.0f});
    m_menuBg.setFillColor(sf::Color(20, 20, 60));
    m_menuBg.setOutlineColor(sf::Color::White);
    m_menuBg.setOutlineThickness(2.0f);

    // ── Menu text ────────────────────────────────────────────────
    m_menuText.setFont(m_font);
    m_menuText.setString("USPICIO");
    m_menuText.setCharacterSize(64);
    m_menuText.setFillColor(sf::Color::White);
    m_menuText.setStyle(sf::Text::Bold);
}

// ────────────────────────────────────────────────────────────────────
void Game::run() {
    sf::Clock clock;
    float accumulator = 0.0f;

    while (m_running && m_renderer.isOpen()) {
        const float frameTime = clock.restart().asSeconds();
        accumulator += (frameTime > MAX_FRAME) ? MAX_FRAME : frameTime;

        processInput();

        // Fixed-timestep updates
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
            // Global: ESC always quits
            if (event.key == core::KeyCode::Escape) {
                m_running = false;
                m_renderer.close();
                return;
            }

            // Menu input
            if (m_state == State::Menu) {
                if (event.key == core::KeyCode::Enter) {
                    setState(State::Playing);
                }
            }
        }
    }
}

// ────────────────────────────────────────────────────────────────────
void Game::update(float /*deltaTime*/) {
    // Reserved for game-object updates (physics, AI, etc.).
}

// ────────────────────────────────────────────────────────────────────
void Game::render() {
    m_renderer.clear(core::Color{10, 10, 30});

    if (m_state == State::Menu) {
        const auto wSz = m_renderer.getSize();

        m_menuBg.setPosition({
            (static_cast<float>(wSz.x) - m_menuBg.getSize().x) / 2.0f,
            (static_cast<float>(wSz.y) - m_menuBg.getSize().y) / 2.0f
        });

        const sf::FloatRect textBounds = m_menuText.getLocalBounds();
        m_menuText.setOrigin(
            textBounds.left + textBounds.width  / 2.0f,
            textBounds.top  + textBounds.height / 2.0f
        );
        m_menuText.setPosition({
            static_cast<float>(wSz.x) / 2.0f,
            static_cast<float>(wSz.y) / 2.0f
        });

        // Temporary coupling: cast to SfmlRenderer to draw SFML
        // objects directly. Will be replaced when game objects
        // implement core::Drawable in later sprints.
        auto& sfmlR = dynamic_cast<infrastructure::SfmlRenderer&>(m_renderer);
        sfmlR.drawSfml(m_menuBg);
        sfmlR.drawSfml(m_menuText);
    }

    m_renderer.display();
}

// ────────────────────────────────────────────────────────────────────
void Game::setState(State newState) {
    m_state = newState;
}

} // namespace gameplay
