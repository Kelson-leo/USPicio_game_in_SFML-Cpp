#include "infrastructure/SfmlRenderer.h"
#include "infrastructure/SfmlInput.h"
#include "gameplay/Game.h"

int main() {
    infrastructure::SfmlRenderer renderer;
    if (!renderer.open({800, 600}, "Uspicio Game")) {
        return 1;
    }

    infrastructure::SfmlInput input(renderer.getSfmlWindow());
    gameplay::Game game(renderer, input);
    game.run();

    return 0;
}
