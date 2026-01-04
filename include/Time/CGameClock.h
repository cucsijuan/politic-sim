#pragma once

#include "Time/CTimeUnits.h"
#include <string>

namespace PoliticSim {

struct SGameTime
{
    float totalSeconds;
    int32_t years;
    int32_t months;
    int32_t days;
    int32_t hours;
    int32_t minutes;
    int32_t seconds;

    SGameTime()
        : totalSeconds(0.0f)
        , years(0)
        , months(0)
        , days(0)
        , hours(0)
        , minutes(0)
        , seconds(0)
    {
    }

    void Normalize()
    {
        // Only normalize units that are being tracked
        if (CTimeUnits::ShouldTrackSeconds())
        {
            seconds = static_cast<int32_t>(totalSeconds) % CTimeUnits::SECONDS_PER_MINUTE;
        }
        else
        {
            seconds = 0;
        }

        if (CTimeUnits::ShouldTrackMinutes())
        {
            minutes = (static_cast<int32_t>(totalSeconds) / CTimeUnits::SECONDS_PER_MINUTE) % CTimeUnits::MINUTES_PER_HOUR;
        }
        else
        {
            minutes = 0;
        }

        if (CTimeUnits::ShouldTrackHours())
        {
            hours = (static_cast<int32_t>(totalSeconds) / CTimeUnits::SECONDS_PER_HOUR) % CTimeUnits::HOURS_PER_DAY;
        }
        else
        {
            hours = 0;
        }

        if (CTimeUnits::ShouldTrackDays())
        {
            days = (static_cast<int32_t>(totalSeconds) / CTimeUnits::SECONDS_PER_DAY) % CTimeUnits::DAYS_PER_MONTH;
        }
        else
        {
            days = 0;
        }

        if (CTimeUnits::ShouldTrackMonths())
        {
            months = (static_cast<int32_t>(totalSeconds) / CTimeUnits::SECONDS_PER_MONTH) % CTimeUnits::MONTHS_PER_YEAR;
        }
        else
        {
            months = 0;
        }

        years = static_cast<int32_t>(totalSeconds) / CTimeUnits::SECONDS_PER_YEAR;
    }

    std::string ToString() const
    {
        return CTimeUnits::FormatDateTime(totalSeconds);
    }

    std::string ToDateString() const
    {
        return CTimeUnits::FormatDate(totalSeconds);
    }

    std::string ToTimeString() const
    {
        return CTimeUnits::FormatTime(totalSeconds);
    }
};

class CGameClock
{
private:
    float m_ElapsedGameTime;
    float m_ElapsedRealTime;
    SGameTime m_CurrentTime;
    float m_DeltaGameTime;

public:
    CGameClock();
    ~CGameClock() = default;

    // Time progression
    void Update(float realDeltaTime, float timeMultiplier);
    void Reset();

    // Query current time
    const SGameTime& GetCurrentTime() const { return m_CurrentTime; }
    float GetElapsedGameTime() const { return m_ElapsedGameTime; }
    float GetElapsedRealTime() const { return m_ElapsedRealTime; }
    float GetDeltaGameTime() const { return m_DeltaGameTime; }

    // Query specific components
    int32_t GetYear() const { return m_CurrentTime.years; }
    int32_t GetMonth() const { return m_CurrentTime.months; }
    int32_t GetDay() const { return m_CurrentTime.days; }
    int32_t GetHour() const { return m_CurrentTime.hours; }
    int32_t GetMinute() const { return m_CurrentTime.minutes; }
    int32_t GetSecond() const { return m_CurrentTime.seconds; }

    // Utility
    std::string GetFormattedDate() const;
    std::string GetFormattedTime() const;
    std::string GetFormattedDateTime() const;
};

} // namespace PoliticSim
