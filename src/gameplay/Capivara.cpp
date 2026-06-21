#include "gameplay/Capivara.h"
#include "gameplay/Player.h"
#include <algorithm>

namespace gameplay {

Capivara::Capivara(const sf::Texture& texture,
                   infrastructure::FrameConfig& frameConfig,
                   float startX)
    : m_sprite(texture)
    , m_frameConfig(frameConfig)
    , m_position{startX, core::GROUND_Y - FRAME_HEIGHT} {
    health.maxHP     = 30;
    health.currentHP = 30;
    m_sprite.setScale(SCALE, SCALE);
    setAnimation("walk");
}

// ────────────────────────────────────────────────────────────────────
// Animation
// ────────────────────────────────────────────────────────────────────

std::string Capivara::buildAnimName(const std::string& action) const {
    return action + "_"
        + (m_direction == core::Direction::Right ? "right" : "left");
}

void Capivara::setAnimation(const std::string& action) {
    const std::string fullName = buildAnimName(action);

    if (fullName == m_currentAnim) return;

    auto count = m_frameConfig.frameCount("capivara", fullName);
    if (count == 0) return;

    m_currentAnim = fullName;
    m_frameIndex  = 0;
    m_frameTimer  = 0.0f;

    auto rect = m_frameConfig.getFrame("capivara", m_currentAnim, 0);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
}

// ────────────────────────────────────────────────────────────────────
// Update
// ────────────────────────────────────────────────────────────────────

void Capivara::update(float dt, const Player& player) {
    if (!m_isAlive) return;

    // ── Animation timer ──────────────────────────────────────────
    if (!m_currentAnim.empty()) {
        m_frameTimer += dt;
        if (m_frameTimer >= FRAME_DURATION) {
            m_frameTimer = 0.0f;

            auto count = m_frameConfig.frameCount("capivara", m_currentAnim);
            if (count > 0) {
                ++m_frameIndex;
                if (m_frameIndex >= count) {
                    bool isWalk = m_currentAnim.find("walk") != std::string::npos;
                    bool isHurt = m_currentAnim.find("hurt") != std::string::npos;
                    if (isWalk) {
                        m_frameIndex = 0;  // loop walk
                    } else if (isHurt) {
                        // Hurt finishes, return to walk
                        setAnimation("walk");
                        // Apply frame 0 immediately
                        auto rect = m_frameConfig.getFrame("capivara", m_currentAnim, 0);
                        m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                                            {rect.width, rect.height}));
                        goto skipFrameUpdate;
                    } else {
                        m_frameIndex = count - 1;  // stay on last (dead)
                    }
                }

                auto rect = m_frameConfig.getFrame("capivara", m_currentAnim, m_frameIndex);
                m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                                    {rect.width, rect.height}));
            }
        }
    }
    skipFrameUpdate:

    // ── AI: move toward player ───────────────────────────────────
    auto pPos = player.getPosition();
    float dx = pPos.x - m_position.x;

    if (dx > 20.0f) {
        m_direction = core::Direction::Right;
    } else if (dx < -20.0f) {
        m_direction = core::Direction::Left;
    }

    // Apply movement
    if (m_direction == core::Direction::Right) {
        m_position.x += SPEED * dt;
    } else {
        m_position.x -= SPEED * dt;
    }

    // Clamp to screen edges
    if (m_position.x < SCREEN_MIN_X) {
        m_position.x = SCREEN_MIN_X;
        m_direction = core::Direction::Right;
    } else if (m_position.x > SCREEN_MAX_X) {
        m_position.x = SCREEN_MAX_X;
        m_direction = core::Direction::Left;
    }

    // Keep animation in sync with direction
    setAnimation("walk");

    // ── Gravity + final position ─────────────────────────────────
    applyGravity(dt);
    m_sprite.setPosition(m_position.x, m_position.y);

    // Contact damage handled by Game::update()
}

// ────────────────────────────────────────────────────────────────────
// Damage
// ────────────────────────────────────────────────────────────────────

void Capivara::takeDamage(int amount) {
    if (!m_isAlive) return;

    health.takeDamage(amount);
    if (health.isDead()) {
        m_isAlive = false;
        setAnimation("dead");
    } else {
        setAnimation("hurt");
    }
}

void Capivara::touchPlayer(Player& player, const core::DamageConfig& cfg) {
    if (!m_isAlive) return;
    player.takeHit(core::AttackType::EnemyTouch, cfg);
}

// ────────────────────────────────────────────────────────────────────
// Gravity
// ────────────────────────────────────────────────────────────────────

void Capivara::applyGravity(float dt) {
    m_velocityY += core::GRAVITY * dt;
    m_position.y += m_velocityY * dt;

    float feetY = m_position.y + FRAME_HEIGHT;
    if (feetY >= core::GROUND_Y) {
        m_position.y = core::GROUND_Y - FRAME_HEIGHT;
        m_velocityY = 0.0f;
    }
}

// ────────────────────────────────────────────────────────────────────
// Draw
// ────────────────────────────────────────────────────────────────────

void Capivara::draw(core::IRenderer& renderer) const {
    if (m_currentAnim.empty()) return;

    // ── Dark outline (8-directional, 1px) ─────────────────────────
    auto& sfSprite = m_sprite.getSfmlSprite();
    const auto origColor = sfSprite.getColor();
    const auto origPos   = sfSprite.getPosition();

    sfSprite.setColor(sf::Color(0, 0, 0, 180));
    const sf::Vector2f offsets[] = {
        {-1, -1}, {0, -1}, {1, -1},
        {-1,  0},          {1,  0},
        {-1,  1}, {0,  1}, {1,  1}
    };
    for (const auto& off : offsets) {
        sfSprite.setPosition(origPos.x + off.x, origPos.y + off.y);
        m_sprite.draw(renderer);
    }

    // ── Main sprite on top ────────────────────────────────────────
    sfSprite.setColor(origColor);
    sfSprite.setPosition(origPos);
    m_sprite.draw(renderer);
}

// ────────────────────────────────────────────────────────────────────
// State
// ────────────────────────────────────────────────────────────────────

bool Capivara::isDead() const {
    return !m_isAlive;
}

sf::Vector2f Capivara::getPosition() const {
    return m_position;
}

void Capivara::setPosition(sf::Vector2f pos) {
    m_position = pos;
    m_sprite.setPosition(pos.x, pos.y);
}

} // namespace gameplay
