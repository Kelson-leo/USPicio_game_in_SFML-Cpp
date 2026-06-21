#include "gameplay/Player.h"
#include <algorithm>

namespace gameplay {

Player::Player(const sf::Texture& texture,
               infrastructure::FrameConfig& frameConfig)
    : m_sprite(texture)
    , m_frameConfig(frameConfig)
    , m_position{100.0f, core::GROUND_Y - PLAYER_HEIGHT} {
    m_sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
    setAnimation("idle");
}

// ────────────────────────────────────────────────────────────────────
// Animation
// ────────────────────────────────────────────────────────────────────

std::string Player::buildAnimName(const std::string& action) const {
    return action + "_"
        + (m_direction == core::Direction::Right ? "right" : "left");
}

void Player::setAnimation(const std::string& action) {
    const std::string fullName = buildAnimName(action);

    // Idempotent: don't reset if already playing this animation
    if (fullName == m_currentAnim) return;

    auto count = m_frameConfig.frameCount("player", fullName);
    if (count == 0) return;  // fallback: keep current animation

    m_currentAnim = fullName;
    m_frameIndex  = 0;
    m_frameTimer  = 0.0f;

    auto rect = m_frameConfig.getFrame("player", m_currentAnim, 0);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
}

void Player::updateAnimation(float dt) {
    // Decrement shoot cooldown
    if (m_shootCooldown > 0.0f) {
        m_shootCooldown -= dt;
    }

    if (m_currentAnim.empty()) return;

    m_frameTimer += dt;
    if (m_frameTimer < FRAME_DURATION) return;
    m_frameTimer = 0.0f;

    auto count = m_frameConfig.frameCount("player", m_currentAnim);
    if (count == 0) return;

    // Advance frame; loop for walk, stop at last for others
    ++m_frameIndex;
    if (m_frameIndex >= count) {
        if (m_currentAnim.find("walk") != std::string::npos) {
            m_frameIndex = 0;  // loop walk
        } else if (m_currentAnim.find("punch") != std::string::npos ||
                   m_currentAnim.find("throw") != std::string::npos) {
            // Attack animations: play once, then return to idle
            setAnimation("idle");
            return;
        } else {
            m_frameIndex = count - 1;  // stay on last frame (jump, defend)
        }
    }

    auto rect = m_frameConfig.getFrame("player", m_currentAnim, m_frameIndex);
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
}

bool Player::isAttacking() const {
    return m_currentAnim.find("punch") != std::string::npos ||
           m_currentAnim.find("throw") != std::string::npos;
}

// ────────────────────────────────────────────────────────────────────
// Draw
// ────────────────────────────────────────────────────────────────────

void Player::draw(core::IRenderer& renderer) const {
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
// Direction
// ────────────────────────────────────────────────────────────────────

core::Direction Player::getDirection() const {
    return m_direction;
}

void Player::setDirection(core::Direction dir) {
    if (m_direction == dir) return;
    m_direction = dir;

    // Re-apply current action with new direction
    // Extract action from current anim name (e.g. "idle_right" → "idle")
    auto pos = m_currentAnim.rfind('_');
    if (pos != std::string::npos) {
        std::string action = m_currentAnim.substr(0, pos);
        setAnimation(action);
    }
}

// ────────────────────────────────────────────────────────────────────
// Offense
// ────────────────────────────────────────────────────────────────────

void Player::punch(core::HealthComponent& enemyHealth,
                   core::EntityType enemyType,
                   const core::DamageConfig& cfg) {
    int dmg = cfg.getDamage(core::AttackType::Punch, enemyType);
    enemyHealth.takeDamage(dmg);
}

void Player::throwCaneta(core::HealthComponent& enemyHealth,
                         core::EntityType enemyType,
                         const core::DamageConfig& cfg) {
    if (!ammo.canUse()) return;

    ammo.use();
    int dmg = cfg.getDamage(core::AttackType::Throw, enemyType);
    enemyHealth.takeDamage(dmg);
}

void Player::throwProjectile(std::vector<std::unique_ptr<Projectile>>& projectiles,
                             infrastructure::FrameConfig& frameConfig,
                             const sf::Texture& texture) {
    if (!ammo.canUse()) return;
    if (m_shootCooldown > 0.0f) return;

    ammo.use();
    m_shootCooldown = SHOOT_COOLDOWN;

    auto proj = std::make_unique<Projectile>();
    sf::Vector2f offset = (m_direction == core::Direction::Right)
        ? sf::Vector2f(40.0f, 20.0f)
        : sf::Vector2f(-10.0f, 20.0f);
    proj->init(ProjectileType::Pen, m_direction, texture,
               frameConfig, m_position + offset);
    projectiles.push_back(std::move(proj));
}

// ────────────────────────────────────────────────────────────────────
// Defense
// ────────────────────────────────────────────────────────────────────

void Player::defend(bool active) {
    m_isDefending = active;
}

bool Player::isDefending() const {
    return m_isDefending;
}

// ────────────────────────────────────────────────────────────────────
// Damage
// ────────────────────────────────────────────────────────────────────

void Player::takeHit(core::AttackType attack,
                     const core::DamageConfig& cfg) {
    int dmg = cfg.getDamage(attack, core::EntityType::Player);
    health.takeDamage(dmg, m_isDefending);

    if (health.isDead()) {
        lives.loseLife();
    }
}

bool Player::revive() {
    if (lives.isGameOver()) return false;

    health.currentHP = health.maxHP;
    m_position = {100.0f, core::GROUND_Y - PLAYER_HEIGHT};
    return true;
}

// ────────────────────────────────────────────────────────────────────
// Movement
// ────────────────────────────────────────────────────────────────────

void Player::moveLeft(float dt) {
    m_position.x -= WALK_SPEED * dt;
    m_sprite.setPosition(m_position.x, m_position.y);
}

void Player::moveRight(float dt) {
    m_position.x += WALK_SPEED * dt;
    m_sprite.setPosition(m_position.x, m_position.y);
}

void Player::applyGravity(float dt) {
    velocityY += core::GRAVITY * dt;
    m_position.y += velocityY * dt;

    // Feet (bottom of sprite) at GROUND_Y
    float feetY = m_position.y + PLAYER_HEIGHT;
    if (feetY >= core::GROUND_Y) {
        m_position.y = core::GROUND_Y - PLAYER_HEIGHT;
        velocityY = 0.0f;
    }
    m_sprite.setPosition(m_position.x, m_position.y);
}

bool Player::isOnGround() const {
    return (m_position.y + PLAYER_HEIGHT) >= core::GROUND_Y;
}

// ────────────────────────────────────────────────────────────────────
// Position
// ────────────────────────────────────────────────────────────────────

sf::Vector2f Player::getPosition() const {
    return m_position;
}

void Player::setPosition(sf::Vector2f pos) {
    m_position = pos;
    m_sprite.setPosition(pos.x, pos.y);
}

} // namespace gameplay
