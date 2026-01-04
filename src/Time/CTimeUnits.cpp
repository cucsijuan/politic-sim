#include "Time/CTimeUnits.h"
#include <sstream>
#include <iomanip>

namespace PoliticSim {

// Static member initialization
ETimeGranularity CTimeUnits::s_Granularity = ETimeGranularity::Months;

void CTimeUnits::SetGranularity(ETimeGranularity granularity)
{
    s_Granularity = granularity;
}

ETimeGranularity CTimeUnits::GetGranularity()
{
    return s_Granularity;
}

bool CTimeUnits::ShouldTrackSeconds()
{
    return s_Granularity == ETimeGranularity::Seconds;
}

bool CTimeUnits::ShouldTrackMinutes()
{
    return s_Granularity == ETimeGranularity::Seconds ||
           s_Granularity == ETimeGranularity::Minutes;
}

bool CTimeUnits::ShouldTrackHours()
{
    return s_Granularity == ETimeGranularity::Seconds ||
           s_Granularity == ETimeGranularity::Minutes ||
           s_Granularity == ETimeGranularity::Hours;
}

bool CTimeUnits::ShouldTrackDays()
{
    return s_Granularity == ETimeGranularity::Seconds ||
           s_Granularity == ETimeGranularity::Minutes ||
           s_Granularity == ETimeGranularity::Hours ||
           s_Granularity == ETimeGranularity::Days;
}

bool CTimeUnits::ShouldTrackMonths()
{
    return s_Granularity == ETimeGranularity::Seconds ||
           s_Granularity == ETimeGranularity::Minutes ||
           s_Granularity == ETimeGranularity::Hours ||
           s_Granularity == ETimeGranularity::Days ||
           s_Granularity == ETimeGranularity::Months;
}

float CTimeUnits::GameSecondsToGameMinutes(float gameSeconds)
{
    return gameSeconds / static_cast<float>(SECONDS_PER_MINUTE);
}

float CTimeUnits::GameMinutesToGameSeconds(float gameMinutes)
{
    return gameMinutes * static_cast<float>(SECONDS_PER_MINUTE);
}

float CTimeUnits::GameSecondsToGameHours(float gameSeconds)
{
    return gameSeconds / static_cast<float>(SECONDS_PER_HOUR);
}

float CTimeUnits::GameHoursToGameSeconds(float gameHours)
{
    return gameHours * static_cast<float>(SECONDS_PER_HOUR);
}

float CTimeUnits::GameSecondsToGameDays(float gameSeconds)
{
    return gameSeconds / static_cast<float>(SECONDS_PER_DAY);
}

float CTimeUnits::GameDaysToGameSeconds(float gameDays)
{
    return gameDays * static_cast<float>(SECONDS_PER_DAY);
}

float CTimeUnits::GameSecondsToGameWeeks(float gameSeconds)
{
    return gameSeconds / static_cast<float>(SECONDS_PER_WEEK);
}

float CTimeUnits::GameWeeksToGameSeconds(float gameWeeks)
{
    return gameWeeks * static_cast<float>(SECONDS_PER_WEEK);
}

float CTimeUnits::GameSecondsToGameMonths(float gameSeconds)
{
    return gameSeconds / static_cast<float>(SECONDS_PER_MONTH);
}

float CTimeUnits::GameMonthsToGameSeconds(float gameMonths)
{
    return gameMonths * static_cast<float>(SECONDS_PER_MONTH);
}

float CTimeUnits::GameSecondsToGameYears(float gameSeconds)
{
    return gameSeconds / static_cast<float>(SECONDS_PER_YEAR);
}

float CTimeUnits::GameYearsToGameSeconds(float gameYears)
{
    return gameYears * static_cast<float>(SECONDS_PER_YEAR);
}

std::string CTimeUnits::FormatTime(float totalSeconds)
{
    std::ostringstream oss;

    int32_t hours = static_cast<int32_t>(totalSeconds / SECONDS_PER_HOUR) % 24;
    int32_t minutes = static_cast<int32_t>(totalSeconds / SECONDS_PER_MINUTE) % 60;
    int32_t seconds = static_cast<int32_t>(totalSeconds) % 60;

    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;

    return oss.str();
}

std::string CTimeUnits::FormatDate(float totalSeconds)
{
    std::ostringstream oss;

    int32_t years = static_cast<int32_t>(totalSeconds / SECONDS_PER_YEAR);
    float remaining = totalSeconds - (years * SECONDS_PER_YEAR);

    int32_t months = 0;
    if (ShouldTrackMonths())
    {
        months = static_cast<int32_t>(remaining / SECONDS_PER_MONTH);
        remaining -= (months * SECONDS_PER_MONTH);
    }

    int32_t days = 0;
    if (ShouldTrackDays())
    {
        days = static_cast<int32_t>(remaining / SECONDS_PER_DAY);
    }

    oss << "Year " << (years + 1);  // Years start at 1, not 0

    if (ShouldTrackMonths())
    {
        oss << ", Month " << (months + 1);
    }

    if (ShouldTrackDays())
    {
        oss << ", Day " << (days + 1);
    }

    return oss.str();
}

std::string CTimeUnits::FormatDateTime(float totalSeconds)
{
    std::ostringstream oss;

    oss << FormatDate(totalSeconds);

    if (ShouldTrackHours())
    {
        oss << " " << FormatTime(totalSeconds);
    }

    return oss.str();
}

} // namespace PoliticSim
