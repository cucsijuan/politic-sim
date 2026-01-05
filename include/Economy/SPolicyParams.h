#pragma once

#include <cstdint>

namespace PoliticSim {

// Economic policy parameters (editable via UI, bypassing politics for now)
struct SPolicyParams
{
    // Tax rates (percentage, 0-100)
    float m_CorporateTaxRate;      // Default: 21.0f
    float m_LaborTaxRate;          // Default: 15.0f

    // Labor regulations
    float m_MinimumWage;           // Default: 7.25f (dollars/hour)
    float m_LaborRegulationBurden; // Default: 0.3f (0-1 scale)
                                   // Represents: firing costs, unemployment insurance, union power, etc.

    // Environmental regulations
    float m_EnvironmentalComplianceCost; // Default: 0.2f (0-1 scale)
    bool m_StrictEnvironmentalPolicy;    // Default: false

    // Business support
    float m_SubsidyRate;           // Default: 0.0f (0-10% of costs)
    bool m_SubsidiesEnabled;       // Default: false

    // Trade policy
    float m_TariffRate;            // Default: 0.0f (0-50%)

    SPolicyParams()
        : m_CorporateTaxRate(21.0f)
        , m_LaborTaxRate(15.0f)
        , m_MinimumWage(7.25f)
        , m_LaborRegulationBurden(0.3f)
        , m_EnvironmentalComplianceCost(0.2f)
        , m_StrictEnvironmentalPolicy(false)
        , m_SubsidyRate(0.0f)
        , m_SubsidiesEnabled(false)
        , m_TariffRate(0.0f)
    {
    }
};

} // namespace PoliticSim
