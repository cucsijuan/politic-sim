#pragma once

#include "Time/CGameClock.h"
#include "Time/CTimeScale.h"
#include "Time/CTimeUnits.h"
#include <cstdint>

namespace PoliticSim {

class CTimeManager
{
private:
    CGameClock m_Clock;
    CTimeScale m_TimeScale;

    // Statistics
    float m_TotalRealTime;
    uint32_t m_FrameCount;
    float m_AverageFPS;

    // Base time configuration: 1 month = X real seconds
    float m_BaseUnitRealSeconds;  // How many real seconds = 1 base unit (month)

public:
    CTimeManager();
    ~CTimeManager() = default;

    // Initialization
    void Initialize();
    void Shutdown();

    // Main update (called from CPoliticalGame::Update)
    void Update(float realDeltaTime);

    // Speed control
    void SetSpeed(ETimeSpeed speed);
    void SetSpeedByIndex(size_t index);
    void IncreaseSpeed();
    void DecreaseSpeed();
    void TogglePause();
    ETimeSpeed GetCurrentSpeed() const;
    float GetTimeMultiplier() const;
    bool IsPaused() const;

    // Time query
    const CGameClock& GetClock() const { return m_Clock; }
    const SGameTime& GetCurrentTime() const;
    float GetDeltaGameTime() const;
    float GetElapsedGameTime() const;

    // Convenience: convert real-time to game-time
    float ConvertRealToGameTime(float realSeconds) const;
    float ConvertGameToRealTime(float gameSeconds) const;

    // Statistics
    float GetTotalRealTime() const { return m_TotalRealTime; }
    uint32_t GetFrameCount() const { return m_FrameCount; }
    float GetAverageFPS() const;

    // Debug/UI support
    std::string GetDebugInfo() const;
    std::string GetSpeedDisplayLabel() const;
    const std::string& GetSpeedName() const;
    const std::vector<STimeSpeedConfig>& GetAllSpeeds() const;
    size_t GetCurrentSpeedIndex() const;

    // Granularity control
    void SetGranularity(ETimeGranularity granularity);
    ETimeGranularity GetGranularity() const;
};

} // namespace PoliticSim
