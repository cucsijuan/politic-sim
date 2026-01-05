#include "Economy/CEconomyManager.h"
#include "Economy/CCompany.h"
#include "Time/CTimeUnits.h"
#include <iostream>
#include <random>

namespace PoliticSim {

CEconomyManager::CEconomyManager()
    : m_Companies()
    , m_PolicyParams()
    , m_MacroState()
    , m_NextCompanyID(1)
    , m_SimulationAccumulator(0.0f)
    , m_TotalEmployment(0.0f)
    , m_TotalGDP(0.0f)
    , m_AverageProfitability(0.0f)
{
}

CEconomyManager::~CEconomyManager()
{
    // Empty - unique_ptr handles cleanup automatically
    // This must be in .cpp where CCompany is fully defined
}

void CEconomyManager::Initialize()
{
    std::cout << "Economy Manager: Initializing..." << std::endl;

    // Create 250 companies across sectors and sizes
    InitializeCompanies();

    // Calculate initial macro state
    UpdateMacroState();

    std::cout << "Economy Manager: Initialized (" << m_Companies.size() << " companies)" << std::endl;
}

void CEconomyManager::Shutdown()
{
    std::cout << "Economy Manager: Shutting down..." << std::endl;
    m_Companies.clear();
    std::cout << "Economy Manager: Shutdown complete" << std::endl;
}

void CEconomyManager::Update(float gameDelta)
{
    // Accumulate game time
    // We need 30 game days = 1 simulation month
    float gameDays = gameDelta / static_cast<float>(CTimeUnits::SECONDS_PER_DAY);
    m_SimulationAccumulator += gameDays;

    // When we've accumulated 30 days, run a simulation tick
    if (m_SimulationAccumulator >= 30.0f)
    {
        SimulateAllCompanies();
        UpdateMacroState();
        m_SimulationAccumulator -= 30.0f;
    }
}

void CEconomyManager::InitializeCompanies()
{
    // Create 250 companies across sectors and sizes
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sectorDist(0, static_cast<int32_t>(ESector::COUNT) - 1);
    std::uniform_int_distribution<> sizeDist(0, 3);

    for (int32_t i = 0; i < 250; ++i)
    {
        // Random sector
        ESector sector = static_cast<ESector>(sectorDist(gen));

        // Random size (weighted toward smaller companies)
        int32_t sizeRoll = sizeDist(gen);
        ECompanySize size;
        if (sizeRoll == 0)
            size = ECompanySize::Micro;
        else if (sizeRoll <= 2)
            size = ECompanySize::Small;
        else if (sizeRoll == 3)
            size = ECompanySize::Medium;
        else
            size = ECompanySize::Large;

        // Create attributes based on sector
        SCompanyAttributes attrs;
        attrs.m_Sector = sector;
        attrs.m_Size = size;

        // Sector-specific attributes
        // Productivity: Revenue generated per employee per month (in thousands)
        // Balanced for ~15-25% profit margin with neutral policies
        switch (sector)
        {
            case ESector::Agriculture:
                attrs.m_BaseProductivity = 18.0f;  // Lower value-added
                attrs.m_LaborIntensity = 0.8f;
                attrs.m_MarketCompetitiveness = 0.6f;
                break;
            case ESector::Industry:
                attrs.m_BaseProductivity = 32.0f;  // Manufacturing efficiency
                attrs.m_LaborIntensity = 0.4f;
                attrs.m_MarketCompetitiveness = 0.5f;
                break;
            case ESector::Services:
                attrs.m_BaseProductivity = 22.0f;  // Service-based
                attrs.m_LaborIntensity = 0.7f;
                attrs.m_MarketCompetitiveness = 0.8f;
                break;
            case ESector::Technology:
                attrs.m_BaseProductivity = 45.0f;  // High value-added
                attrs.m_LaborIntensity = 0.3f;
                attrs.m_MarketCompetitiveness = 0.6f;
                break;
            case ESector::Retail:
                attrs.m_BaseProductivity = 20.0f;  // Volume-based, low margin
                attrs.m_LaborIntensity = 0.9f;
                attrs.m_MarketCompetitiveness = 0.9f;
                break;
        }

        // Create company
        std::string name = "Company_" + std::to_string(m_NextCompanyID);
        m_Companies.push_back(std::make_unique<CCompany>(m_NextCompanyID, name, attrs));
        m_NextCompanyID++;
    }
}

void CEconomyManager::SimulateAllCompanies()
{
    // Simulate each company for one month
    for (const auto& company : m_Companies)
    {
        if (company)
        {
            company->SimulateMonth(m_PolicyParams, m_MacroState);
        }
    }
}

void CEconomyManager::UpdateMacroState()
{
    // Calculate aggregates from all companies
    float totalEmployees = 0.0f;
    float totalRevenue = 0.0f;
    float totalProfit = 0.0f;
    float totalWages = 0.0f;

    size_t companyCount = m_Companies.size();
    if (companyCount == 0)
    {
        return;
    }

    for (const auto& company : m_Companies)
    {
        if (company)
        {
            totalEmployees += static_cast<float>(company->GetEmployees());
            totalRevenue += company->GetMonthlyRevenue();
            totalProfit += company->GetProfitability();
            totalWages += company->GetWageLevel();
        }
    }

    // Update aggregates
    m_TotalEmployment = totalEmployees;
    m_TotalGDP = totalRevenue;
    m_AverageProfitability = totalProfit / static_cast<float>(companyCount);

    // Update macro state
    m_MacroState.m_AverageWage = totalWages / static_cast<float>(companyCount);

    // Unemployment rate (simplified: assume workforce = 2x employment)
    float workforce = totalEmployees * 2.0f;
    m_MacroState.m_UnemploymentRate = ((workforce - totalEmployees) / workforce) * 100.0f;

    // Business confidence (based on profitability)
    if (m_AverageProfitability > 10.0f)
    {
        m_MacroState.m_BusinessConfidence = 70.0f;
    }
    else if (m_AverageProfitability > 0.0f)
    {
        m_MacroState.m_BusinessConfidence = 55.0f;
    }
    else if (m_AverageProfitability > -10.0f)
    {
        m_MacroState.m_BusinessConfidence = 40.0f;
    }
    else
    {
        m_MacroState.m_BusinessConfidence = 25.0f;
    }

    // Aggregate demand (function of employment and confidence)
    m_MacroState.m_AggregateDemand = (totalEmployees / workforce) *
                                     (m_MacroState.m_BusinessConfidence / 50.0f);

    // Calculate sector-specific metrics for market saturation
    int32_t sectorCompanyCounts[static_cast<int32_t>(ESector::COUNT)] = {0};
    float sectorRevenue[static_cast<int32_t>(ESector::COUNT)] = {0.0f};

    for (const auto& company : m_Companies)
    {
        if (company)
        {
            ESector sector = company->GetAttributes().m_Sector;
            int32_t sectorIndex = static_cast<int32_t>(sector);
            sectorCompanyCounts[sectorIndex]++;
            sectorRevenue[sectorIndex] += company->GetMonthlyRevenue();
        }
    }

    // Calculate saturation and import competition for each sector
    for (int32_t i = 0; i < static_cast<int32_t>(ESector::COUNT); ++i)
    {
        // Saturation based on company count (50 companies = 0.5, 100+ = 1.0)
        float companySaturation = std::min(1.0f, static_cast<float>(sectorCompanyCounts[i]) / 100.0f);

        // Saturation also based on total revenue in sector (50M revenue = saturated)
        float revenueSaturation = std::min(1.0f, sectorRevenue[i] / 50000.0f);

        // Combined saturation (average of both factors)
        m_MacroState.m_SectorSaturation[i] = (companySaturation + revenueSaturation) / 2.0f;

        // Policy-dependent import competition
        // Base competition varies by sector (structural factors)
        float baseImportCompetition = 0.3f; // Default baseline
        if (i == static_cast<int32_t>(ESector::Retail))
            baseImportCompetition = 0.6f;  // Retail very import-dependent
        else if (i == static_cast<int32_t>(ESector::Technology))
            baseImportCompetition = 0.5f;  // Tech vulnerable to imports
        else if (i == static_cast<int32_t>(ESector::Industry))
            baseImportCompetition = 0.4f;  // Industry moderately vulnerable
        else if (i == static_cast<int32_t>(ESector::Agriculture))
            baseImportCompetition = 0.35f; // Ag less vulnerable
        else if (i == static_cast<int32_t>(ESector::Services))
            baseImportCompetition = 0.2f;  // Services least import-dependent

        // Tariffs reduce import competition (protectionism)
        // At 50% tariff, import competition is reduced by 50%
        float tariffProtection = m_PolicyParams.m_TariffRate / 100.0f;
        m_MacroState.m_ImportCompetition[i] = baseImportCompetition * (1.0f - tariffProtection);
    }
}

} // namespace PoliticSim
