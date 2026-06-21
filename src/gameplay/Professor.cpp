#include "gameplay/Professor.h"
#include "gameplay/Player.h"

namespace gameplay {

void Professor::shootBook(Player& player, const core::DamageConfig& cfg) {
    player.takeHit(core::AttackType::BossProjectile, cfg);
}

} // namespace gameplay
