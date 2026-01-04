#pragma once

#include <string>
#include <cstdint>

namespace PoliticSim {

enum class ETimeUnit
{
    Second,
    Minute,
    Hour,
    Day,
    Week,
    Month,
    Year,
    Decade
};

enum class ETimeGranularity
{
    Seconds,      // Track: years → months → days → hours → minutes → seconds
    Minutes,      // Track: years → months → days → hours → minutes (seconds = 0)
    Hours,        // Track: years → months → days → hours (minutes = 0)
    Days,         // Track: years → months → days (hours = 0)
    Months,       // Track: years → months (days = 0) ← Default for this game
    Years         // Track: years only (months = 0)
};

class CTimeUnits
{
private:
    static ETimeGranularity s_Granularity;

public:
    // Time unit conversion constants (simplified 30-day months for gameplay)
    static constexpr int32_t SECONDS_PER_MINUTE = 60;
    static constexpr int32_t MINUTES_PER_HOUR = 60;
    static constexpr int32_t HOURS_PER_DAY = 24;
    static constexpr int32_t DAYS_PER_WEEK = 7;
    static constexpr int32_t DAYS_PER_MONTH = 30;  // Simplified for gameplay
    static constexpr int32_t MONTHS_PER_YEAR = 12;
    static constexpr int32_t WEEKS_PER_YEAR = 52;

    // Derived constants
    static constexpr int32_t SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR;
    static constexpr int32_t SECONDS_PER_DAY = SECONDS_PER_HOUR * HOURS_PER_DAY;
    static constexpr int32_t SECONDS_PER_WEEK = SECONDS_PER_DAY * DAYS_PER_WEEK;
    static constexpr int32_t SECONDS_PER_MONTH = SECONDS_PER_DAY * DAYS_PER_MONTH;
    static constexpr int32_t SECONDS_PER_YEAR = SECONDS_PER_MONTH * MONTHS_PER_YEAR;
    static constexpr int32_t SECONDS_PER_DECADE = SECONDS_PER_YEAR * 10;

    // Granularity control
    static void SetGranularity(ETimeGranularity granularity);
    static ETimeGranularity GetGranularity();
    static bool ShouldTrackSeconds();
    static bool ShouldTrackMinutes();
    static bool ShouldTrackHours();
    static bool ShouldTrackDays();
    static bool ShouldTrackMonths();

    // Conversion: Game time units ↔ Game seconds
    static float GameSecondsToGameMinutes(float gameSeconds);
    static float GameMinutesToGameSeconds(float gameMinutes);
    static float GameSecondsToGameHours(float gameSeconds);
    static float GameHoursToGameSeconds(float gameHours);
    static float GameSecondsToGameDays(float gameSeconds);
    static float GameDaysToGameSeconds(float gameDays);
    static float GameSecondsToGameWeeks(float gameSeconds);
    static float GameWeeksToGameSeconds(float gameWeeks);
    static float GameSecondsToGameMonths(float gameSeconds);
    static float GameMonthsToGameSeconds(float gameMonths);
    static float GameSecondsToGameYears(float gameSeconds);
    static float GameYearsToGameSeconds(float gameYears);

    // Formatting
    static std::string FormatTime(float totalSeconds);
    static std::string FormatDate(float totalSeconds);
    static std::string FormatDateTime(float totalSeconds);
};

} // namespace PoliticSim
