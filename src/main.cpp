#include "infrastructure/SfmlRenderer.h"
#include "infrastructure/SfmlInput.h"
#include "infrastructure/AssetManager.h"
#include "gameplay/Game.h"
#include <SFML/Window/WindowContext.hpp>
#include <SFML/Graphics/GraphicsContext.hpp>
#include <iostream>

int main() {
    std::cout << "[main] start" << std::endl;

    std::cout << "[main] creating WindowContext..." << std::endl;
    auto windowContext = sf::WindowContext::create().value();
    std::cout << "[main] WindowContext OK" << std::endl;

    std::cout << "[main] creating GraphicsContext..." << std::endl;
    auto graphicsContext = sf::GraphicsContext::create().value();
    std::cout << "[main] GraphicsContext OK" << std::endl;

    {
        std::cout << "[main] creating renderer..." << std::endl;
        infrastructure::SfmlRenderer renderer;
        if (!renderer.open({1920, 1080}, "Uspicio Game")) {
            std::cerr << "[main] renderer.open FAILED" << std::endl;
            return 1;
        }
        std::cout << "[main] renderer opened OK" << std::endl;

        std::cout << "[main] creating input..." << std::endl;
        infrastructure::SfmlInput input(renderer.getSfmlWindow());
        std::cout << "[main] input OK" << std::endl;

        std::cout << "[main] creating Game..." << std::endl;
        gameplay::Game game(renderer, input);
        std::cout << "[main] Game created, entering run loop..." << std::endl;

        game.run();
        std::cout << "[main] game.run() returned" << std::endl;
    }

    // Clear singletons before graphics subsystem is destroyed to avoid
    // texture cleanup attempting to use an already-destroyed GL context.
    infrastructure::AssetManager::instance().clear();
    std::cout << "[main] exit OK" << std::endl;

    return 0;
}
