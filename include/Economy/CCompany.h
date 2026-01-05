#pragma once

#include "Economy/ECompanyTypes.h"
#include "Economy/SCompanyState.h"
#include "Economy/SCompanyAttributes.h"
#include "Economy/SPolicyParams.h"
#include "Economy/SMacroState.h"
#include <cstdint>
#include <string>

namespace PoliticSim {

class CCompany
{
private:
    uint32_t m_ID;
    std::string m_Name;

    SCompanyAttributes m_Attributes;  // What the company IS (static)
    SCompanyState m_State;            // How the company IS DOING (dynamic)

    // History for expectations (last 6 months)
    static constexpr int32_t HISTORY_MONTHS = 24;
    float m_ProfitHistory[HISTORY_MONTHS];
    float m_EmployeesHistory[HISTORY_MONTHS];
    float m_LiquidityHistory[HISTORY_MONTHS];
    float m_RevenueHistory[HISTORY_MONTHS];
    int32_t m_HistoryIndex;

    // Internal helpers
    void CalculateRevenue(const SMacroState& macro);
    void CalculateCosts(const SPolicyParams& policy, const SMacroState& macro);
    void UpdateExpectations();
    void MakeDecisions(const SPolicyParams& policy, const SMacroState& macro);
    void CheckBankruptcy();
    void UpdateHistory();

public:
    CCompany(uint32_t id, const std::string& name, const SCompanyAttributes& attributes);
    ~CCompany() = default;

    // Main simulation
    void SimulateMonth(const SPolicyParams& policy, const SMacroState& macro);

    // Accessors
    uint32_t GetID() const { return m_ID; }
    const std::string& GetName() const { return m_Name; }
    const SCompanyState& GetState() const { return m_State; }
    const SCompanyAttributes& GetAttributes() const { return m_Attributes; }

    // Query helpers
    bool IsProfitable() const { return m_State.m_Profitability > 0.0f; }
    bool IsInCrisis() const { return m_State.m_State == ECompanyState::Crisis; }
    float GetMonthlyRevenue() const { return m_State.m_LastRevenue; }
    int32_t GetEmployees() const { return m_State.m_Employees; }
    float GetProfitability() const { return m_State.m_Profitability; }
    float GetWageLevel() const { return m_State.m_WageLevel; }

    // History access
    static constexpr int32_t GetHistoryMonths() { return HISTORY_MONTHS; }
    const float* GetProfitHistory() const { return m_ProfitHistory; }
    const float* GetEmployeesHistory() const { return m_EmployeesHistory; }
    const float* GetLiquidityHistory() const { return m_LiquidityHistory; }
    const float* GetRevenueHistory() const { return m_RevenueHistory; }
    int32_t GetHistoryIndex() const { return m_HistoryIndex; }
};

} // namespace PoliticSim
