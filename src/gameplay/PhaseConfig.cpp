#include "gameplay/PhaseConfig.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

const PhaseData PhaseConfig::s_empty{};

bool PhaseConfig::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "[PhaseConfig] Cannot open " << path << "\n";
        return false;
    }

    try {
        nlohmann::json json;
        in >> json;

        m_phases.clear();
        for (const auto& jf : json.at("fases")) {
            PhaseData pd;
            pd.id         = jf.at("id").get<int>();
            pd.background = jf.at("background").get<std::string>();
            pd.enemyCount = jf.at("inimigos_comuns").get<int>();
            pd.enemyType  = jf.at("tipo_inimigo").get<std::string>();
            pd.hasBoss    = jf.at("chefao").get<bool>();
            pd.groundY    = jf.value("ground_y", 900.0f);
            if (pd.hasBoss && jf.contains("chefao_tipo")) {
                pd.bossType = jf.at("chefao_tipo").get<std::string>();
            }
            m_phases.push_back(std::move(pd));
        }

        return true;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "[PhaseConfig] JSON parse error: " << e.what() << "\n";
        m_phases.clear();
        return false;
    }
}

int PhaseConfig::size() const {
    return static_cast<int>(m_phases.size());
}

const PhaseData& PhaseConfig::getPhase(int index) const {
    if (index < 0 || index >= size())
        return s_empty;
    return m_phases[index];
}

const std::string& PhaseConfig::getBackground(int index) const {
    return getPhase(index).background;
}

int PhaseConfig::getEnemyCount(int index) const {
    return getPhase(index).enemyCount;
}

bool PhaseConfig::hasBoss(int index) const {
    return getPhase(index).hasBoss;
}

const std::string& PhaseConfig::getBossType(int index) const {
    return getPhase(index).bossType;
}

float PhaseConfig::getGroundY(int index) const {
    return getPhase(index).groundY;
}
