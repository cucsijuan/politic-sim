#pragma once

#include <cstdint>

namespace PoliticSim {

// Company sectors (simplified set for vertical slice)
enum class ESector
{
    Agriculture,
    Industry,
    Services,
    Technology,
    Retail,

    // Count of sectors (for array sizing)
    COUNT = 5
};

// Company size categories
enum class ECompanySize
{
    Micro,      // 0-10 employees
    Small,      // 11-50 employees
    Medium,     // 51-250 employees
    Large       // 251+ employees
};

// Company state for decision making
enum class ECompanyState
{
    Growing,        // Profitable, expanding
    Stable,         // Maintaining, steady
    Declining,      // Struggling, cutting costs
    Crisis          // Near bankruptcy
};

} // namespace PoliticSim
