#pragma once

#include "Economy/SPolicyParams.h"
#include "Economy/SMacroState.h"
#include <vector>
#include <memory>
#include <cstdint>

namespace PoliticSim {

// Forward declaration
class CCompany;

class CEconomyManager
{
private:
    std::vector<std::unique_ptr<CCompany>> m_Companies;
    SPolicyParams m_PolicyParams;
    SMacroState m_MacroState;

    uint32_t m_NextCompanyID;
    float m_SimulationAccumulator;  // Track game time for monthly ticks

    // Aggregates (calculated from companies)
    float m_TotalEmployment;
    float m_TotalGDP;
    float m_AverageProfitability;

    // Internal helpers
    void InitializeCompanies();
    void UpdateMacroState();
    void SimulateAllCompanies();

public:
    CEconomyManager();
    ~CEconomyManager(); // Not default - needs full CCompany type definition

    // Lifecycle
    void Initialize();
    void Shutdown();

    // Main update (called from game loop, receives game delta time)
    void Update(float gameDelta);

    // Policy access (for UI)
    SPolicyParams& GetPolicyParams() { return m_PolicyParams; }
    const SPolicyParams& GetPolicyParams() const { return m_PolicyParams; }

    // Macro state access (read-only, calculated internally)
    const SMacroState& GetMacroState() const { return m_MacroState; }

    // Company access (for UI)
    const std::vector<std::unique_ptr<CCompany>>& GetCompanies() const { return m_Companies; }
    size_t GetCompanyCount() const { return m_Companies.size(); }

    // Aggregates (for UI)
    float GetTotalEmployment() const { return m_TotalEmployment; }
    float GetTotalGDP() const { return m_TotalGDP; }
    float GetAverageProfitability() const { return m_AverageProfitability; }
    float GetUnemploymentRate() const { return m_MacroState.m_UnemploymentRate; }
};

} // namespace PoliticSim
