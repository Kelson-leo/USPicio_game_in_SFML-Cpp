#include "Game.h"
#include <SFML/Window/Event.hpp>
#include <iostream>

namespace gameplay {

// ────────────────────────────────────────────────────────────────────
Game::Game(core::IRenderer& renderer, core::IInputHandler& input)
    : m_renderer(renderer)
    , m_input(input) {
    // ── Load default font ─────────────────────────────────────────
    // SFML 2.6 no longer bundles a default font; we use a
    // hard-coded path to the system DejaVu font available on
    // Debian/Ubuntu. In production this will come from assets/.
    if (!m_font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        std::cerr << "[Game] WARNING: Could not load font. "
                     "Text will not render.\n";
    }

    // ── Menu background ───────────────────────────────────────────
    m_menuBg.setSize({400.0f, 150.0f});
    m_menuBg.setFillColor(sf::Color(20, 20, 60));
    m_menuBg.setOutlineColor(sf::Color::White);
    m_menuBg.setOutlineThickness(2.0f);

    // ── Menu text ─────────────────────────────────────────────────
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
        if (frameTime > MAX_FRAME) {
            accumulator += MAX_FRAME;
        } else {
            accumulator += frameTime;
        }

        processInput();

        // Fixed-timestep updates
        while (accumulator >= FIXED_DT) {
            update(FIXED_DT);
            accumulator -= FIXED_DT;
        }

        // Variable-timestep render (with interpolation alpha if needed)
        render();
    }
}

// ────────────────────────────────────────────────────────────────────
void Game::processInput() {
    sf::Event event;
    while (m_input.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_running = false;
            m_renderer.close();
            return;
        }

        // ── Keyboard input ────────────────────────────────────────
        if (event.type == sf::Event::KeyPressed) {
            // Global: ESC always quits
            if (event.key.code == sf::Keyboard::Key::Escape) {
                m_running = false;
                m_renderer.close();
                return;
            }

            // Menu input (placeholder)
            if (m_state == State::Menu) {
                if (event.key.code == sf::Keyboard::Key::Enter) {
                    setState(State::Playing);
                }
            }
        }
    }
}

// ────────────────────────────────────────────────────────────────────
void Game::update(float /*deltaTime*/) {
    // Reserved for game-object updates (physics, AI, etc.).
    // Menu state has no per-frame logic for now.
}

// ────────────────────────────────────────────────────────────────────
void Game::render() {
    m_renderer.clear(sf::Color(10, 10, 30));

    if (m_state == State::Menu) {
        // Centre the background and text on the window
        const auto& win = m_renderer.getWindow();
        const auto  wSz = win.getSize();

        m_menuBg.setPosition({
            (wSz.x - m_menuBg.getSize().x) / 2.0f,
            (wSz.y - m_menuBg.getSize().y) / 2.0f
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

        m_renderer.draw(m_menuBg);
        m_renderer.draw(m_menuText);
    }

    // Playing state will draw game objects here.

    m_renderer.display();
}

// ────────────────────────────────────────────────────────────────────
void Game::setState(State newState) {
    m_state = newState;
}

} // namespace gameplay
