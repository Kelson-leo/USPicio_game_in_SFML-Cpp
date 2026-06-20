#include "DamageConfig.h"

namespace core {

DamageConfig::DamageConfig() {
    m_table[{AttackType::Punch,          EntityType::Capivara}]  = 10;
    m_table[{AttackType::Punch,          EntityType::Professor}] = 5;
    m_table[{AttackType::Throw,          EntityType::Capivara}]  = 20;
    m_table[{AttackType::Throw,          EntityType::Professor}] = 15;
    m_table[{AttackType::EnemyTouch,     EntityType::Player}]    = 8;
    m_table[{AttackType::BossProjectile, EntityType::Player}]    = 12;
}

int DamageConfig::getDamage(AttackType attack, EntityType target) const {
    auto it = m_table.find({attack, target});
    return (it != m_table.end()) ? it->second : 0;
}

void DamageConfig::setDamage(AttackType attack, EntityType target, int amount) {
    m_table[{attack, target}] = amount;
}

} // namespace core
