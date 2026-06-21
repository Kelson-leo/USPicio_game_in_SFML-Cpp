#pragma once

#include <string>
#include <vector>

/// Lightweight value object for a single phase's configuration.
struct PhaseData {
    int id               = 0;
    std::string background;       ///< path relative to executable
    float groundY        = 900.0f; ///< ground level for this phase
    int  enemyCount       = 0;
    std::string enemyType;        ///< e.g. "capivara"
    bool hasBoss          = false;
    std::string bossType;         ///< e.g. "professor"
};

/// Loads and provides access to the phase progression configuration
/// stored in assets/config/fases.json.
class PhaseConfig {
public:
    /// Load configuration from a JSON file.
    /// @return true on success.
    bool loadFromFile(const std::string& path);

    /// Number of phases defined.
    int size() const;

    /// Returns the full PhaseData for the given 0-based index.
    /// Returns a default-constructed PhaseData if out of range.
    const PhaseData& getPhase(int index) const;

    /// Convenience: background path for the given phase index.
    const std::string& getBackground(int index) const;

    /// Convenience: number of common enemies for the given phase index.
    int getEnemyCount(int index) const;

    /// Convenience: whether the given phase has a boss.
    bool hasBoss(int index) const;

    /// Convenience: boss type string (e.g. "professor").
    const std::string& getBossType(int index) const;

    /// Convenience: ground Y for the given phase index.
    float getGroundY(int index) const;

private:
    std::vector<PhaseData> m_phases;

    /// Sentinel returned when an out-of-range index is requested.
    static const PhaseData s_empty;
};
