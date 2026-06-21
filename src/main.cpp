#include "infrastructure/SfmlRenderer.h"
#include "infrastructure/SfmlInput.h"
#include "infrastructure/AssetManager.h"
#include "gameplay/Game.h"
#include <SFML/Window/WindowContext.hpp>
#include <SFML/Graphics/GraphicsContext.hpp>

int main() {
    auto windowContext = sf::WindowContext::create().value();
    auto graphicsContext = sf::GraphicsContext::create().value();

    {
        infrastructure::SfmlRenderer renderer;
        if (!renderer.open({1920, 1080}, "Uspicio Game")) {
            return 1;
        }

        infrastructure::SfmlInput input(renderer.getSfmlWindow());
        gameplay::Game game(renderer, input);
        game.run();
    }

    // Clear singletons before graphics subsystem is destroyed to avoid
    // texture cleanup attempting to use an already-destroyed GL context.
    infrastructure::AssetManager::instance().clear();

    return 0;
}
