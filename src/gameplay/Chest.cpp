#include "gameplay/Chest.h"
#include "gameplay/Player.h"
#include <cmath>

namespace gameplay {

Chest::Chest() = default;

void Chest::init(const sf::Texture& texture,
                 infrastructure::FrameConfig& config,
                 const sf::Vector2f& position,
                 float groundY) {
    m_frameConfig = &config;
    m_groundY     = groundY;
    m_wasOpened   = false;

    m_sprite = infrastructure::SfmlSprite(texture);

    auto rect = m_frameConfig->getFrame("chest", "closed", 0);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
    m_position = position;
    m_sprite.setPosition(m_position.x, m_position.y);
}

void Chest::update(Player& player) {
    if (m_wasOpened) return;

    // Simple bounding-box collision
    auto pPos = player.getPosition();
    float playerH = player.getCurrentHeight();
    sf::FloatRect playerBounds(pPos.x, pPos.y, 60.0f, playerH);

    sf::FloatRect chestBounds(m_position.x, m_position.y,
                              CHEST_WIDTH, CHEST_HEIGHT);

    if (playerBounds.intersects(chestBounds)) {
        tryOpen(player);
    }
}

void Chest::tryOpen(Player& player) {
    // Only activate if player is below threshold in lives OR ammo
    bool needsHeal = (static_cast<float>(player.lives.currentLives)
                      < player.lives.maxLives * THRESHOLD)
                  || (static_cast<float>(player.ammo.currentAmmo)
                      < player.ammo.maxAmmo * THRESHOLD);
    if (!needsHeal) return;

    m_wasOpened = true;

    // ── Restore lives ──────────────────────────────────────────────
    int lostLives = player.lives.maxLives - player.lives.currentLives;
    int restoreLives = static_cast<int>(std::ceil(lostLives * HEAL_PERCENT));
    if (restoreLives > 0) {
        player.lives.restoreLives(restoreLives);
    }

    // ── Restore ammo ───────────────────────────────────────────────
    int lostAmmo = player.ammo.maxAmmo - player.ammo.currentAmmo;
    int restoreAmmo = static_cast<int>(std::ceil(lostAmmo * HEAL_PERCENT));
    if (restoreAmmo > 0) {
        player.ammo.reload(restoreAmmo);
    }

    // ── Switch to open frame ───────────────────────────────────────
    if (m_frameConfig) {
        auto rect = m_frameConfig->getFrame("chest", "open", 0);
        m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                            {rect.width, rect.height}));
    }
}

void Chest::draw(core::IRenderer& renderer) const {
    m_sprite.draw(renderer);
}

} // namespace gameplay
