#include "Time/CTimeScale.h"
#include <algorithm>

namespace PoliticSim {

CTimeScale::CTimeScale()
    : m_CurrentSpeed(ETimeSpeed::Slow)
    , m_CurrentSpeedIndex(1)
{
    SetDefaultSpeeds();
}

void CTimeScale::SetDefaultSpeeds()
{
    m_SpeedLevels.clear();

    // Default configuration for month-based gameplay
    // Base: 1 month = 5 real seconds (at 1x speed)
    // Multipliers are relative to this base

    // Paused: No time progression
    m_SpeedLevels.push_back(STimeSpeedConfig(
        ETimeSpeed::Paused, "Paused", 0.0f, "||"));

    // Slow: 1x speed (1 month / 5 real seconds)
    m_SpeedLevels.push_back(STimeSpeedConfig(
        ETimeSpeed::Slow, "Slow", 1.0f, ">"));

    // Normal: 2x speed (1 month / 2.5 real seconds)
    m_SpeedLevels.push_back(STimeSpeedConfig(
        ETimeSpeed::Normal, "Normal", 2.0f, ">>"));

    // Fast: 5x speed (1 month / 1 real second)
    m_SpeedLevels.push_back(STimeSpeedConfig(
        ETimeSpeed::Fast, "Fast", 5.0f, ">>>"));

    // Very Fast: 10x speed (1 month / 0.5 real seconds = 2 months/second)
    m_SpeedLevels.push_back(STimeSpeedConfig(
        ETimeSpeed::VeryFast, "Very Fast", 10.0f, ">>>>"));

    // Maximum: 30x speed (1 month / 0.167 real seconds ≈ 6 months/second)
    m_SpeedLevels.push_back(STimeSpeedConfig(
        ETimeSpeed::Maximum, "Maximum", 30.0f, ">>>>>"));

    // Start at Slow (1x)
    SetSpeed(ETimeSpeed::Slow);
}

void CTimeScale::AddSpeedLevel(const STimeSpeedConfig& config)
{
    m_SpeedLevels.push_back(config);
}

void CTimeScale::SetSpeed(ETimeSpeed speed)
{
    for (size_t i = 0; i < m_SpeedLevels.size(); ++i)
    {
        if (m_SpeedLevels[i].speed == speed)
        {
            m_CurrentSpeed = speed;
            m_CurrentSpeedIndex = i;
            return;
        }
    }
}

void CTimeScale::SetSpeedByIndex(size_t index)
{
    if (index < m_SpeedLevels.size())
    {
        m_CurrentSpeedIndex = index;
        m_CurrentSpeed = m_SpeedLevels[index].speed;
    }
}

void CTimeScale::IncreaseSpeed()
{
    if (m_CurrentSpeedIndex < m_SpeedLevels.size() - 1)
    {
        SetSpeedByIndex(m_CurrentSpeedIndex + 1);
    }
}

void CTimeScale::DecreaseSpeed()
{
    if (m_CurrentSpeedIndex > 0)
    {
        SetSpeedByIndex(m_CurrentSpeedIndex - 1);
    }
}

void CTimeScale::TogglePause()
{
    if (IsPaused())
    {
        // Resume: go back to Slow (1x)
        SetSpeed(ETimeSpeed::Slow);
    }
    else
    {
        // Pause
        SetSpeed(ETimeSpeed::Paused);
    }
}

float CTimeScale::GetTimeMultiplier() const
{
    if (m_CurrentSpeedIndex < m_SpeedLevels.size())
    {
        return m_SpeedLevels[m_CurrentSpeedIndex].multiplier;
    }
    return 0.0f;
}

const std::string& CTimeScale::GetSpeedName() const
{
    if (m_CurrentSpeedIndex < m_SpeedLevels.size())
    {
        return m_SpeedLevels[m_CurrentSpeedIndex].name;
    }
    static std::string empty = "";
    return empty;
}

const std::string& CTimeScale::GetSpeedLabel() const
{
    if (m_CurrentSpeedIndex < m_SpeedLevels.size())
    {
        return m_SpeedLevels[m_CurrentSpeedIndex].displayLabel;
    }
    static std::string empty = "";
    return empty;
}

} // namespace PoliticSim
