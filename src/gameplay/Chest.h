#pragma once

#include "core/Drawable.h"
#include "core/PhysicsConstants.h"
#include "infrastructure/SfmlSprite.h"
#include "infrastructure/FrameConfig.h"
#include <SFML/System/Vector2.hpp>

namespace gameplay {

class Player;

/// Interactive chest that restores lives and ammo on first touch.
/// Placed in Phase 5 (Sanfran). Single-use — after opening, stays open.
class Chest : public core::Drawable {
public:
    Chest();

    void init(const sf::Texture& texture,
              infrastructure::FrameConfig& config,
              const sf::Vector2f& position,
              float groundY);

    /// Checks collision with player and opens if touched for the first time.
    void update(Player& player);

    void draw(core::IRenderer& renderer) const override;

    bool isOpen() const { return m_wasOpened; }
    sf::Vector2f getPosition() const { return m_position; }

    static constexpr float CHEST_SCALE   = 0.3f;
    static constexpr float CHEST_FRAME_W = 267.0f;
    static constexpr float CHEST_FRAME_H = 223.0f;
    static constexpr float CHEST_WIDTH   = CHEST_FRAME_W * CHEST_SCALE;  // ~80
    static constexpr float CHEST_HEIGHT  = CHEST_FRAME_H * CHEST_SCALE;  // ~67
    static constexpr float HEAL_PERCENT  = 0.8f;
    static constexpr float THRESHOLD     = 0.8f;

private:
    /// Opens the chest only if player is below threshold in lives OR ammo.
    void tryOpen(Player& player);

    infrastructure::SfmlSprite   m_sprite;
    infrastructure::FrameConfig* m_frameConfig = nullptr;
    sf::Vector2f                 m_position;
    float                        m_groundY    = core::GROUND_Y;
    bool                         m_wasOpened  = false;
};

} // namespace gameplay
