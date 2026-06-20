#pragma once

#include "core/DamageTypes.h"
#include <unordered_map>
#include <utility>

namespace core {

/// Stores base-damage values for each AttackType x EntityType pair.
///
/// Values are adjustable via setters so tuning does not require
/// recompilation of dependent translation units (future JSON migration
/// is possible by swapping this class's implementation).
class DamageConfig {
public:
    DamageConfig();

    int getDamage(AttackType attack, EntityType target) const;

    void setDamage(AttackType attack, EntityType target, int amount);

private:
    struct PairHash {
        std::size_t operator()(std::pair<AttackType, EntityType> p) const {
            return static_cast<std::size_t>(p.first) * 10 +
                   static_cast<std::size_t>(p.second);
        }
    };

    std::unordered_map<std::pair<AttackType, EntityType>, int, PairHash> m_table;
};

} // namespace core
