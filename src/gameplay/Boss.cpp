#include "gameplay/Boss.h"
#include "gameplay/Player.h"
#include <cmath>

namespace gameplay {

Boss::Boss(const sf::Texture& texture,
           infrastructure::FrameConfig& frameConfig,
           const std::string& bossName,
           float startX)
    : m_sprite(texture)
    , m_frameConfig(frameConfig)
    , m_bossName(bossName)
    , m_position{startX, m_groundY - BOSS_HEIGHT} {
    health.maxHP     = 80;
    health.currentHP = 80;

    auto rect = m_frameConfig.getFrame(m_bossName, "idle_right", 0);
    if (rect.width == 0) {
        rect = m_frameConfig.getFrame(m_bossName, "idle", 0);
    }
    m_sprite.setTextureRect(sf::IntRect({rect.left, rect.top},
                                        {rect.width, rect.height}));
    m_sprite.setPosition(m_position.x, m_position.y);
}

void Boss::draw(core::IRenderer& renderer) const {
    m_sprite.draw(renderer);
}

void Boss::update(float dt, const Player& player,
                  std::vector<std::unique_ptr<Projectile>>& projectiles,
                  infrastructure::FrameConfig& frameConfig) {
    if (health.isDead()) return;

    float dx = std::abs(player.getPosition().x - m_position.x);

    // ── Melee attack (if enabled and in range) ──────────────────
    if (m_canMelee && dx <= MELEE_RANGE) {
        m_meleeCooldown -= dt;
        if (m_meleeCooldown <= 0.0f) {
            performMeleeAttack(const_cast<Player&>(player));
            m_meleeCooldown = MELEE_COOLDOWN;
        }
        return;  // melee takes priority over ranged
    }

    // ── Ranged attack ───────────────────────────────────────────
    // Minimum distance prevents spawning inside the player (instant collision → invisible)
    if (dx <= RANGED_RANGE && dx > 80.0f && m_projectileTexture) {
        m_rangedCooldown -= dt;
        if (m_rangedCooldown <= 0.0f) {
            shootProjectile(projectiles, frameConfig, player);
        }
    }
}

ProjectileType Boss::getProjectileType() const {
    return ProjectileType::Exam;  // default: professor
}

void Boss::shootProjectile(
    std::vector<std::unique_ptr<Projectile>>& projectiles,
    infrastructure::FrameConfig& frameConfig,
    const Player& player) {

    if (health.isDead() || !m_projectileTexture) return;

    auto pPos = player.getPosition();
    core::Direction dir = (pPos.x > m_position.x)
        ? core::Direction::Right
        : core::Direction::Left;

    auto proj = std::make_unique<Projectile>();
    sf::Vector2f offset = (dir == core::Direction::Right)
        ? sf::Vector2f(80.0f, 20.0f)
        : sf::Vector2f(-80.0f, 20.0f);
    proj->init(getProjectileType(), dir, *m_projectileTexture,
               frameConfig, m_position + offset);
    projectiles.push_back(std::move(proj));

    m_rangedCooldown = RANGED_COOLDOWN;
}

void Boss::performMeleeAttack(Player& player) {
    // Default: no-op. Override in melee-capable subclasses.
}

sf::Vector2f Boss::getPosition() const {
    return m_position;
}

void Boss::setPosition(sf::Vector2f pos) {
    m_position = pos;
    m_sprite.setPosition(pos.x, pos.y);
}

void Boss::setGroundY(float y) {
    m_groundY = y;
    m_position.y = y - BOSS_HEIGHT;
    m_sprite.setPosition(m_position.x, m_position.y);
}

void Boss::setCanMelee(bool can) {
    m_canMelee = can;
}

void Boss::setProjectileTexture(const sf::Texture& tex) {
    m_projectileTexture = &tex;
}

} // namespace gameplay
