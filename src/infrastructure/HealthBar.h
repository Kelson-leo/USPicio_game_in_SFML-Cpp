#pragma once

#include "core/Drawable.h"
#include "core/HealthComponent.h"
#include "core/IRenderer.h"
#include <SFML/Graphics/RectangleShape.hpp>

namespace infrastructure {

/// Draws a health bar (red background + green fill) above an entity.
/// Implements core::Drawable so it slots into the existing rendering
/// pipeline via dynamic_cast<SfmlRenderer&> (same pattern as SfmlSprite).
class HealthBar : public core::Drawable {
public:
    HealthBar(core::HealthComponent& health,
              core::Vector2f size = {100.0f, 10.0f});

    void draw(core::IRenderer& renderer) const override;

    void setPosition(core::Vector2f position);
    void setSize(core::Vector2f size);

private:
    core::HealthComponent& m_health;
    core::Vector2f         m_position;
    core::Vector2f         m_size;
};

} // namespace infrastructure
