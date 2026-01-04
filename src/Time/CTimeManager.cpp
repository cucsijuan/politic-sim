#include "Time/CTimeManager.h"
#include <sstream>
#include <iomanip>

namespace PoliticSim {

CTimeManager::CTimeManager()
    : m_Clock()
    , m_TimeScale()
    , m_TotalRealTime(0.0f)
    , m_FrameCount(0)
    , m_AverageFPS(60.0f)
    , m_BaseUnitRealSeconds(5.0f)  // Default: 1 month = 5 real seconds
{
}

void CTimeManager::Initialize()
{
    // Set default granularity to Months (political simulation)
    CTimeUnits::SetGranularity(ETimeGranularity::Months);

    // Reset clock
    m_Clock.Reset();

    // Reset statistics
    m_TotalRealTime = 0.0f;
    m_FrameCount = 0;
    m_AverageFPS = 60.0f;
}

void CTimeManager::Shutdown()
{
    // Nothing to clean up currently
}

void CTimeManager::Update(float realDeltaTime)
{
    m_TotalRealTime += realDeltaTime;
    m_FrameCount++;

    // Calculate FPS
    static float fpsUpdateTime = 0.0f;
    static uint32_t fpsFrameCount = 0;
    fpsUpdateTime += realDeltaTime;
    fpsFrameCount++;

    if (fpsUpdateTime >= 1.0f)
    {
        m_AverageFPS = static_cast<float>(fpsFrameCount) / fpsUpdateTime;
        fpsFrameCount = 0;
        fpsUpdateTime = 0.0f;
    }

    // Calculate effective multiplier considering base time unit
    // Base: 1 month = 5 real seconds
    // 1 real second = (30 days * 24 hours * 60 minutes * 60 seconds) / 5 = 518,400 game seconds
    // But we use multipliers relative to this base
    float speedMultiplier = m_TimeScale.GetTimeMultiplier();

    // Convert real seconds to game seconds based on base unit
    // At 1x speed: 1 real second = (SECONDS_PER_MONTH / m_BaseUnitRealSeconds) game seconds
    float baseMultiplier = static_cast<float>(CTimeUnits::SECONDS_PER_MONTH) / m_BaseUnitRealSeconds;
    float effectiveMultiplier = baseMultiplier * speedMultiplier;

    // Update game clock
    m_Clock.Update(realDeltaTime, effectiveMultiplier);
}

void CTimeManager::SetSpeed(ETimeSpeed speed)
{
    m_TimeScale.SetSpeed(speed);
}

void CTimeManager::SetSpeedByIndex(size_t index)
{
    m_TimeScale.SetSpeedByIndex(index);
}

void CTimeManager::IncreaseSpeed()
{
    m_TimeScale.IncreaseSpeed();
}

void CTimeManager::DecreaseSpeed()
{
    m_TimeScale.DecreaseSpeed();
}

void CTimeManager::TogglePause()
{
    m_TimeScale.TogglePause();
}

ETimeSpeed CTimeManager::GetCurrentSpeed() const
{
    return m_TimeScale.GetCurrentSpeed();
}

float CTimeManager::GetTimeMultiplier() const
{
    return m_TimeScale.GetTimeMultiplier();
}

bool CTimeManager::IsPaused() const
{
    return m_TimeScale.IsPaused();
}

const SGameTime& CTimeManager::GetCurrentTime() const
{
    return m_Clock.GetCurrentTime();
}

float CTimeManager::GetDeltaGameTime() const
{
    return m_Clock.GetDeltaGameTime();
}

float CTimeManager::GetElapsedGameTime() const
{
    return m_Clock.GetElapsedGameTime();
}

float CTimeManager::ConvertRealToGameTime(float realSeconds) const
{
    float baseMultiplier = static_cast<float>(CTimeUnits::SECONDS_PER_MONTH) / m_BaseUnitRealSeconds;
    float speedMultiplier = m_TimeScale.GetTimeMultiplier();
    return realSeconds * baseMultiplier * speedMultiplier;
}

float CTimeManager::ConvertGameToRealTime(float gameSeconds) const
{
    float baseMultiplier = static_cast<float>(CTimeUnits::SECONDS_PER_MONTH) / m_BaseUnitRealSeconds;
    float speedMultiplier = m_TimeScale.GetTimeMultiplier();

    if (speedMultiplier > 0.0f)
    {
        return gameSeconds / (baseMultiplier * speedMultiplier);
    }
    return 0.0f;
}

float CTimeManager::GetAverageFPS() const
{
    return m_AverageFPS;
}

std::string CTimeManager::GetDebugInfo() const
{
    std::ostringstream oss;

    oss << "=== Time Manager Debug Info ===\n";
    oss << "Current Time: " << m_Clock.GetFormattedDateTime() << "\n";
    oss << "Game Time Elapsed: " << m_Clock.GetElapsedGameTime() << " seconds\n";
    oss << "Real Time Elapsed: " << m_TotalRealTime << " seconds\n";
    oss << "Speed: " << m_TimeScale.GetSpeedName() << " (" << m_TimeScale.GetSpeedLabel() << ")\n";
    oss << "Multiplier: " << GetTimeMultiplier() << "x\n";
    oss << "FPS: " << std::fixed << std::setprecision(1) << m_AverageFPS << "\n";
    oss << "Frame Count: " << m_FrameCount << "\n";
    oss << "Is Paused: " << (IsPaused() ? "Yes" : "No") << "\n";
    oss << "Granularity: ";

    switch (CTimeUnits::GetGranularity())
    {
        case ETimeGranularity::Seconds: oss << "Seconds"; break;
        case ETimeGranularity::Minutes: oss << "Minutes"; break;
        case ETimeGranularity::Hours: oss << "Hours"; break;
        case ETimeGranularity::Days: oss << "Days"; break;
        case ETimeGranularity::Months: oss << "Months"; break;
        case ETimeGranularity::Years: oss << "Years"; break;
    }

    oss << "\n===============================";

    return oss.str();
}

std::string CTimeManager::GetSpeedDisplayLabel() const
{
    return m_TimeScale.GetSpeedLabel();
}

const std::string& CTimeManager::GetSpeedName() const
{
    return m_TimeScale.GetSpeedName();
}

const std::vector<STimeSpeedConfig>& CTimeManager::GetAllSpeeds() const
{
    return m_TimeScale.GetAllSpeeds();
}

size_t CTimeManager::GetCurrentSpeedIndex() const
{
    return m_TimeScale.GetCurrentSpeedIndex();
}

void CTimeManager::SetGranularity(ETimeGranularity granularity)
{
    CTimeUnits::SetGranularity(granularity);
}

ETimeGranularity CTimeManager::GetGranularity() const
{
    return CTimeUnits::GetGranularity();
}

} // namespace PoliticSim
