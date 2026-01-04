#pragma once

#include <string>
#include <vector>

namespace PoliticSim {

enum class ETimeSpeed
{
    Paused,
    Slow,
    Normal,
    Fast,
    VeryFast,
    Maximum
};

struct STimeSpeedConfig
{
    ETimeSpeed speed;
    std::string name;
    float multiplier;          // Multiplier relative to base time unit
    std::string displayLabel;  // "||", ">", ">>", ">>>"

    STimeSpeedConfig(ETimeSpeed inSpeed, const std::string& inName,
                    float inMultiplier, const std::string& inLabel)
        : speed(inSpeed)
        , name(inName)
        , multiplier(inMultiplier)
        , displayLabel(inLabel)
    {
    }
};

class CTimeScale
{
private:
    std::vector<STimeSpeedConfig> m_SpeedLevels;
    ETimeSpeed m_CurrentSpeed;
    size_t m_CurrentSpeedIndex;

public:
    CTimeScale();
    ~CTimeScale() = default;

    // Configure speed levels (programmer API)
    void AddSpeedLevel(const STimeSpeedConfig& config);
    void SetDefaultSpeeds();

    // Speed control (runtime API)
    void SetSpeed(ETimeSpeed speed);
    void SetSpeedByIndex(size_t index);
    void IncreaseSpeed();
    void DecreaseSpeed();
    void TogglePause();

    // Query current state
    ETimeSpeed GetCurrentSpeed() const { return m_CurrentSpeed; }
    float GetTimeMultiplier() const;
    const std::string& GetSpeedName() const;
    const std::string& GetSpeedLabel() const;
    bool IsPaused() const { return m_CurrentSpeed == ETimeSpeed::Paused; }

    // Access all speeds (for UI)
    const std::vector<STimeSpeedConfig>& GetAllSpeeds() const { return m_SpeedLevels; }
    size_t GetCurrentSpeedIndex() const { return m_CurrentSpeedIndex; }
};

} // namespace PoliticSim
