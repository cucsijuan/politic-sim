#include "Time/CGameClock.h"

namespace PoliticSim {

CGameClock::CGameClock()
    : m_ElapsedGameTime(0.0f)
    , m_ElapsedRealTime(0.0f)
    , m_CurrentTime()
    , m_DeltaGameTime(0.0f)
{
}

void CGameClock::Update(float realDeltaTime, float timeMultiplier)
{
    // Accumulate time
    m_ElapsedRealTime += realDeltaTime;
    m_DeltaGameTime = realDeltaTime * timeMultiplier;
    m_ElapsedGameTime += m_DeltaGameTime;

    // Update SGameTime structure
    m_CurrentTime.totalSeconds = m_ElapsedGameTime;
    m_CurrentTime.Normalize();
}

void CGameClock::Reset()
{
    m_ElapsedGameTime = 0.0f;
    m_ElapsedRealTime = 0.0f;
    m_DeltaGameTime = 0.0f;
    m_CurrentTime = SGameTime();
}

std::string CGameClock::GetFormattedDate() const
{
    return m_CurrentTime.ToDateString();
}

std::string CGameClock::GetFormattedTime() const
{
    return m_CurrentTime.ToTimeString();
}

std::string CGameClock::GetFormattedDateTime() const
{
    return m_CurrentTime.ToString();
}

} // namespace PoliticSim
