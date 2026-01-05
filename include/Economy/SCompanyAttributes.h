#pragma once

#include <cstdint>
#include "Economy/ECompanyTypes.h"

namespace PoliticSim {

// Company structural attributes (slow-changing, define what company IS)
struct SCompanyAttributes
{
    ESector m_Sector;
    ECompanySize m_Size;

    // Productivity factors
    float m_BaseProductivity;      // Output per employee (thousands/month)
    float m_LaborIntensity;        // 0-1 (0=capital intensive, 1=labor intensive)

    // Market characteristics
    float m_MarketCompetitiveness; // 0-1 (0=monopoly, 1=perfect competition)
    float m_DomesticOrientation;   // 0-1 (0=export only, 1=domestic only)

    // Flexibility
    float m_CapitalMobility;       // 0-1 (ease of relocation)

    SCompanyAttributes()
        : m_Sector(ESector::Services)
        , m_Size(ECompanySize::Small)
        , m_BaseProductivity(5.0f)
        , m_LaborIntensity(0.6f)
        , m_MarketCompetitiveness(0.7f)
        , m_DomesticOrientation(0.8f)
        , m_CapitalMobility(0.3f)
    {
    }
};

} // namespace PoliticSim
