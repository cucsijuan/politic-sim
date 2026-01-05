#pragma once

#include <cstdint>
#include "Economy/ECompanyTypes.h"

namespace PoliticSim {

// Company internal state (fast-changing variables)
struct SCompanyState
{
    // Financials
    float m_Liquidity;             // Cash reserves (thousands of dollars)
    float m_Profitability;         // Monthly profit/loss (thousands)
    float m_Debt;                  // Total debt (thousands)
    float m_LastRevenue;           // Revenue from last calculation (for debugging)

    // Operations
    int32_t m_Employees;           // Current workforce
    float m_WageLevel;             // Average wage paid (dollars/hour)
    float m_CapacityUtilization;   // 0-1, how much capacity is used

    // Expectations
    float m_ExpectedProfit;        // 6-month outlook
    float m_PerceivedRisk;         // 0-1, subjective risk assessment

    // Status
    ECompanyState m_State;         // Growing/Stable/Declining/Crisis
    float m_FormalityLevel;        // 0-1, degree of formal operation

    SCompanyState()
        : m_Liquidity(100.0f)
        , m_Profitability(0.0f)
        , m_Debt(0.0f)
        , m_LastRevenue(0.0f)
        , m_Employees(10)
        , m_WageLevel(15.0f)
        , m_CapacityUtilization(0.8f)
        , m_ExpectedProfit(0.0f)
        , m_PerceivedRisk(0.3f)
        , m_State(ECompanyState::Stable)
        , m_FormalityLevel(1.0f)
    {
    }
};

} // namespace PoliticSim
