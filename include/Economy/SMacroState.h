#pragma once

#include <cstdint>
#include "Economy/ECompanyTypes.h"

namespace PoliticSim {

// Macroeconomic indicators (affects all companies)
struct SMacroState
{
    // Aggregates (calculated from companies)
    float m_UnemploymentRate;      // Default: 5.0f (percentage)
    float m_AverageWage;           // Default: 20.0f (dollars/hour)
    float m_BusinessConfidence;    // Default: 50.0f (0-100 index)
    float m_AggregateDemand;       // Default: 1.0f (1.0 = baseline)

    // External factors (set by player/central bank - future)
    float m_InterestRate;          // Default: 3.0f (percentage)
    float m_InflationRate;         // Default: 2.0f (percentage)

    // Market saturation per sector (0-1, where 1 = fully saturated)
    // Index corresponds to ESector enum (0=Ag, 1=Ind, 2=Svc, 3=Tech, 4=Ret)
    float m_SectorSaturation[static_cast<int32_t>(ESector::COUNT)];

    // Import competition per sector (0-1, where 1 = high import pressure)
    // Policy-dependent: affected by tariff rate, trade agreements
    float m_ImportCompetition[static_cast<int32_t>(ESector::COUNT)];

    SMacroState()
        : m_UnemploymentRate(5.0f)
        , m_AverageWage(20.0f)
        , m_BusinessConfidence(50.0f)
        , m_AggregateDemand(1.0f)
        , m_InterestRate(3.0f)
        , m_InflationRate(2.0f)
        , m_SectorSaturation{0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
        , m_ImportCompetition{0.3f, 0.3f, 0.3f, 0.3f, 0.3f}
    {
    }
};

} // namespace PoliticSim
