#include "Economy/CCompany.h"
#include <cmath>
#include <algorithm>

namespace PoliticSim {

CCompany::CCompany(uint32_t id, const std::string& name, const SCompanyAttributes& attributes)
    : m_ID(id)
    , m_Name(name)
    , m_Attributes(attributes)
    , m_State()
    , m_HistoryIndex(0)
{
    // Initialize history to zero
    for (int32_t i = 0; i < HISTORY_MONTHS; ++i)
    {
        m_ProfitHistory[i] = 0.0f;
        m_EmployeesHistory[i] = 0.0f;
        m_LiquidityHistory[i] = 0.0f;
        m_RevenueHistory[i] = 0.0f;
    }

    // Set initial state based on size
    switch (m_Attributes.m_Size)
    {
        case ECompanySize::Micro:
            m_State.m_Employees = 5;
            m_State.m_Liquidity = 20.0f;
            m_State.m_WageLevel = 18.0f;  // Increased from $12
            break;
        case ECompanySize::Small:
            m_State.m_Employees = 25;
            m_State.m_Liquidity = 100.0f;
            m_State.m_WageLevel = 22.0f;  // Increased from $15
            break;
        case ECompanySize::Medium:
            m_State.m_Employees = 150;
            m_State.m_Liquidity = 500.0f;
            m_State.m_WageLevel = 27.0f;  // Increased from $18
            break;
        case ECompanySize::Large:
            m_State.m_Employees = 1000;
            m_State.m_Liquidity = 5000.0f;
            m_State.m_WageLevel = 33.0f;  // Increased from $22
            break;
    }

    // Adjust wage by sector
    switch (m_Attributes.m_Sector)
    {
        case ESector::Agriculture:
            m_State.m_WageLevel *= 0.8f;
            break;
        case ESector::Industry:
            m_State.m_WageLevel *= 1.0f;
            break;
        case ESector::Services:
            m_State.m_WageLevel *= 0.9f;
            break;
        case ESector::Technology:
            m_State.m_WageLevel *= 1.5f;
            break;
        case ESector::Retail:
            m_State.m_WageLevel *= 0.85f;
            break;
    }

    // Set initial capacity utilization based on size
    switch (m_Attributes.m_Size)
    {
        case ECompanySize::Micro:
            m_State.m_CapacityUtilization = 0.7f;
            break;
        case ECompanySize::Small:
            m_State.m_CapacityUtilization = 0.75f;
            break;
        case ECompanySize::Medium:
            m_State.m_CapacityUtilization = 0.8f;
            break;
        case ECompanySize::Large:
            m_State.m_CapacityUtilization = 0.85f;
            break;
    }
}

void CCompany::SimulateMonth(const SPolicyParams& policy, const SMacroState& macro)
{
    // 1. Calculate revenue
    CalculateRevenue(macro);

    // 2. Calculate costs
    CalculateCosts(policy, macro);

    // 3. Update liquidity
    m_State.m_Liquidity += m_State.m_Profitability;

    // 4. Update history and expectations
    UpdateHistory();
    UpdateExpectations();

    // 5. Make decisions (hire/fire, invest, etc.)
    MakeDecisions(policy, macro);

    // 6. Check for bankruptcy
    CheckBankruptcy();
}

void CCompany::CalculateRevenue(const SMacroState& macro)
{
    // Revenue = Employees × BaseProductivity × Demand × CapacityUtilization
    float baseRevenue = static_cast<float>(m_State.m_Employees) *
                       m_Attributes.m_BaseProductivity;

    // Adjust for aggregate demand
    float demandFactor = macro.m_AggregateDemand;

    // Adjust for capacity utilization (can't exceed 100%)
    float utilizationFactor = m_State.m_CapacityUtilization;

    // Adjust for business confidence (affects demand)
    float confidenceFactor = 0.8f + (macro.m_BusinessConfidence / 500.0f); // 0.8-1.0

    float revenue = baseRevenue * demandFactor * utilizationFactor * confidenceFactor;

    // NEW: Apply market saturation and import competition penalties
    int32_t sectorIndex = static_cast<int32_t>(m_Attributes.m_Sector);
    float saturation = macro.m_SectorSaturation[sectorIndex];
    float importCompetition = macro.m_ImportCompetition[sectorIndex];

    // Scale advantage: Large companies handle saturation better (economies of scale)
    float scaleAdvantage = 0.0f;
    switch (m_Attributes.m_Size)
    {
        case ECompanySize::Micro:
            scaleAdvantage = 0.0f;  // No advantage, suffers full penalty
            break;
        case ECompanySize::Small:
            scaleAdvantage = 0.1f;  // 10% penalty reduction
            break;
        case ECompanySize::Medium:
            scaleAdvantage = 0.2f;  // 20% penalty reduction
            break;
        case ECompanySize::Large:
            scaleAdvantage = 0.35f; // 35% penalty reduction (economies of scale)
            break;
    }

    // Saturation reduces revenue potential (max 40% penalty, reduced by scale advantage)
    float effectiveSaturation = std::max(0.0f, saturation - scaleAdvantage);
    float saturationPenalty = 1.0f - (effectiveSaturation * 0.4f); // 0% to 40% penalty
    revenue *= saturationPenalty;

    // Import competition reduces revenue for domestic-focused companies
    if (m_Attributes.m_DomesticOrientation > 0.5f)
    {
        float importPenalty = importCompetition * m_Attributes.m_DomesticOrientation * 0.25f;
        revenue *= (1.0f - importPenalty);
    }

    // Store for cost calculation
    m_State.m_LastRevenue = revenue;
}

void CCompany::CalculateCosts(const SPolicyParams& policy, const SMacroState& macro)
{
    // Labor costs
    // Wage is in dollars/hour, need to convert to thousands of dollars
    float monthlyHours = 160.0f; // 40 hours/week × 4 weeks
    float laborCost = static_cast<float>(m_State.m_Employees) *
                     m_State.m_WageLevel * monthlyHours / 1000.0f; // Convert to thousands

    // Labor tax (social security, etc.)
    laborCost *= (1.0f + policy.m_LaborTaxRate / 100.0f);

    // Regulatory burden (affects labor costs more for labor-intensive firms)
    // Increased impact for more meaningful policy choices
    float regulationCost = laborCost * policy.m_LaborRegulationBurden *
                          m_Attributes.m_LaborIntensity * 1.5f;

    // Environmental compliance (higher impact for strict policy)
    float environmentalCost = 0.0f;
    if (policy.m_StrictEnvironmentalPolicy)
    {
        environmentalCost = laborCost * policy.m_EnvironmentalComplianceCost * 1.0f;
    }
    else
    {
        environmentalCost = laborCost * policy.m_EnvironmentalComplianceCost * 0.3f;
    }

    // Tariff impact (affects companies that depend on imports/exports)
    // Retail and tech are more affected by trade policy
    float tariffImpact = 0.0f;
    if (m_Attributes.m_Sector == ESector::Retail ||
        m_Attributes.m_Sector == ESector::Technology)
    {
        tariffImpact = m_State.m_LastRevenue * (policy.m_TariffRate / 100.0f) * 0.5f;
    }
    else if (m_Attributes.m_Sector == ESector::Industry)
    {
        tariffImpact = m_State.m_LastRevenue * (policy.m_TariffRate / 100.0f) * 0.3f;
    }
    else
    {
        tariffImpact = m_State.m_LastRevenue * (policy.m_TariffRate / 100.0f) * 0.1f;
    }

    // Financial costs (debt interest)
    float financialCost = m_State.m_Debt * (macro.m_InterestRate / 100.0f / 12.0f);

    // Total costs (including tariff impact as a cost)
    float totalCosts = laborCost + regulationCost + environmentalCost +
                      financialCost + tariffImpact;

    // Subsidies (reduce costs)
    float subsidyAmount = 0.0f;
    if (policy.m_SubsidiesEnabled)
    {
        subsidyAmount = totalCosts * (policy.m_SubsidyRate / 100.0f);
    }

    // Corporate tax (on profit)
    float preTaxProfit = m_State.m_LastRevenue - totalCosts + subsidyAmount;
    float taxAmount = 0.0f;
    if (preTaxProfit > 0.0f)
    {
        taxAmount = preTaxProfit * (policy.m_CorporateTaxRate / 100.0f);
    }

    // Final profitability
    m_State.m_Profitability = preTaxProfit - taxAmount;
}

void CCompany::UpdateExpectations()
{
    // Calculate moving average of last 6 months
    float sum = 0.0f;
    int32_t count = 0;
    for (int32_t i = 0; i < HISTORY_MONTHS; ++i)
    {
        // Count ALL history, including zeros (new companies start at 0)
        sum += m_ProfitHistory[i];
        count++;
    }

    if (count > 0)
    {
        float avgProfit = sum / count;

        // Expectation = current trend + momentum
        float trend = (m_State.m_Profitability - avgProfit) / (std::abs(avgProfit) + 0.1f);
        m_State.m_ExpectedProfit = m_State.m_Profitability * (1.0f + trend * 0.3f);
    }
    else
    {
        m_State.m_ExpectedProfit = m_State.m_Profitability;
    }

    // Update perceived risk
    if (m_State.m_Liquidity < 50.0f)
    {
        m_State.m_PerceivedRisk = 0.8f;
    }
    else if (m_State.m_Liquidity < 200.0f)
    {
        m_State.m_PerceivedRisk = 0.5f;
    }
    else
    {
        m_State.m_PerceivedRisk = 0.2f;
    }
}

void CCompany::MakeDecisions(const SPolicyParams& policy, const SMacroState& macro)
{
    // Decision tree based on profitability and expectations

    // NEW: Check market saturation before hiring
    int32_t sectorIndex = static_cast<int32_t>(m_Attributes.m_Sector);
    float saturation = macro.m_SectorSaturation[sectorIndex];

    // High profit + positive expectations + MARKET NOT SATURATED = EXPAND
    if (m_State.m_ExpectedProfit > 10.0f &&
        m_State.m_Liquidity > 200.0f &&
        saturation < 0.85f) // Can't grow if market is 85%+ saturated
    {
        m_State.m_State = ECompanyState::Growing;

        // Growth rate reduced by saturation (companies can't grow fast in saturated markets)
        float growthPotential = std::max(0.0f, 1.0f - (saturation * 1.5f));
        int32_t newHires = static_cast<int32_t>(m_State.m_Employees * 0.05f * growthPotential);
        m_State.m_Employees += newHires;

        // Increase capacity utilization (slower in saturated markets)
        m_State.m_CapacityUtilization = std::min(1.0f, m_State.m_CapacityUtilization + 0.05f * growthPotential);

        // Increase wages slightly to attract workers (only if not already high)
        if (m_State.m_WageLevel < policy.m_MinimumWage * 3.0f)
        {
            m_State.m_WageLevel *= 1.005f;  // 0.5% increase instead of 2%
        }
    }
    // Moderate profit + neutral expectations = STABLE
    else if (m_State.m_Profitability > 0.0f && m_State.m_ExpectedProfit > -5.0f)
    {
        m_State.m_State = ECompanyState::Stable;

        // Maintain current size
        // Small adjustments to capacity
        if (m_State.m_CapacityUtilization > 0.95f)
        {
            m_State.m_CapacityUtilization = 0.95f;
        }
    }
    // Low profit + negative expectations = DECLINE
    // Increased threshold from -5.0f to -15.0f to avoid premature layoffs
    else if (m_State.m_Profitability < -15.0f || m_State.m_ExpectedProfit < -20.0f)
    {
        m_State.m_State = ECompanyState::Declining;

        // Layoffs (5% reduction)
        int32_t layoffs = static_cast<int32_t>(m_State.m_Employees * 0.05f);
        m_State.m_Employees = std::max(1, m_State.m_Employees - layoffs);

        // Reduce capacity
        m_State.m_CapacityUtilization = std::max(0.5f, m_State.m_CapacityUtilization - 0.05f);

        // Freeze or reduce wages
        if (m_State.m_WageLevel > policy.m_MinimumWage)
        {
            m_State.m_WageLevel *= 0.98f;
        }
    }

    // Crisis: Very low liquidity
    if (m_State.m_Liquidity < 20.0f)
    {
        m_State.m_State = ECompanyState::Crisis;

        // Emergency layoffs (10%)
        int32_t emergencyLayoffs = static_cast<int32_t>(m_State.m_Employees * 0.1f);
        m_State.m_Employees = std::max(1, m_State.m_Employees - emergencyLayoffs);

        // Take debt if possible
        if (m_State.m_Debt < m_State.m_Liquidity * 2.0f)
        {
            m_State.m_Debt += 50.0f;  // Borrow 50k
            m_State.m_Liquidity += 50.0f;
        }

        // Consider informalization (evade regulations)
        if (policy.m_LaborRegulationBurden > 0.5f && m_Attributes.m_Size <= ECompanySize::Small)
        {
            m_State.m_FormalityLevel = std::max(0.0f, m_State.m_FormalityLevel - 0.1f);
        }
    }
    else
    {
        // Recover formality if conditions improve
        if (policy.m_LaborRegulationBurden < 0.3f && m_State.m_FormalityLevel < 1.0f)
        {
            m_State.m_FormalityLevel = std::min(1.0f, m_State.m_FormalityLevel + 0.05f);
        }
    }

    // Ensure wage doesn't go below minimum
    if (m_State.m_WageLevel < policy.m_MinimumWage)
    {
        m_State.m_WageLevel = policy.m_MinimumWage;
    }

    // Capital allocation - distribute excess liquidity as dividends or reinvest
    if (m_State.m_Liquidity > 100.0f)  // Only if significant liquidity
    {
        // Calculate target liquidity (6 months operating expenses)
        float monthlyExpenses = static_cast<float>(m_State.m_Employees) *
                                m_State.m_WageLevel * 160.0f / 1000.0f;
        float targetLiquidity = monthlyExpenses * 6.0f;

        float excessLiquidity = m_State.m_Liquidity - targetLiquidity;

        if (excessLiquidity > 0.0f && m_State.m_Profitability > 0.0f)
        {
            // Dividend rate based on company state
            float dividendRate = 0.5f;  // Default 50%

            switch (m_State.m_State)
            {
                case ECompanyState::Growing:
                    dividendRate = 0.4f;  // Retain more for growth
                    break;
                case ECompanyState::Stable:
                    dividendRate = 0.7f;  // Balanced distribution
                    break;
                case ECompanyState::Declining:
                    dividendRate = 0.2f;  // Conserve cash
                    break;
                case ECompanyState::Crisis:
                    dividendRate = 0.0f;  // Keep everything
                    break;
            }

            float dividends = excessLiquidity * dividendRate;
            m_State.m_Liquidity -= dividends;

            // Growing companies: 30% chance to reinvest for productivity boost
            if (m_State.m_State == ECompanyState::Growing && (rand() % 100) < 30)
            {
                float investment = excessLiquidity * 0.3f;
                m_Attributes.m_BaseProductivity *= 1.03f;  // 3% boost
                m_State.m_Liquidity -= investment;
            }
        }
    }
}

void CCompany::CheckBankruptcy()
{
    // Bankruptcy if liquidity is very negative for multiple periods
    if (m_State.m_Liquidity < -100.0f)
    {
        // In full system, company would be destroyed
        // For now, just set to crisis state and stop operations
        m_State.m_State = ECompanyState::Crisis;
        m_State.m_Employees = 0;
        m_State.m_CapacityUtilization = 0.0f;
    }
}

void CCompany::UpdateHistory()
{
    m_ProfitHistory[m_HistoryIndex] = m_State.m_Profitability;
    m_EmployeesHistory[m_HistoryIndex] = static_cast<float>(m_State.m_Employees);
    m_LiquidityHistory[m_HistoryIndex] = m_State.m_Liquidity;
    m_RevenueHistory[m_HistoryIndex] = m_State.m_LastRevenue;
    m_HistoryIndex = (m_HistoryIndex + 1) % HISTORY_MONTHS;
}

} // namespace PoliticSim
