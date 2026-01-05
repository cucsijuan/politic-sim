# PoliticSim: Country System Architecture Plan

## Overview

Design and implement a modular, ECS-inspired country system that serves as the foundation for the entire game. The country contains swappable modules (economy, population, government, laws) with shared context accessible to all systems.

---

## Core Requirements

### 1. Swappable Systems
- **Economy Types**: Capitalist ↔ Socialist ↔ Mixed (can switch mid-game)
- **Government Types**: Parliamentary Republic ↔ Presidential Republic ↔ Authoritarian ↔ Constitutional Monarchy
- **Decision Mechanisms**: Congress deliberation, Presidential decrees, Dictatorial decrees

### 2. Separation of Concerns
- **Policies**: WHAT the country does (tax rates, regulations, spending)
- **Government**: HOW policies are decided/implemented (democratic process, authoritarian decree)
- **Modules**: Systems that AFFECTED BY policies (economy, population, resources)

### 3. Shared Context
- All modules should access common country data (population, policies) without passing it around
- Single source of truth for country state

---

## Proposed Architecture

### Layer 1: Country Context (Shared State)

```cpp
// include/Country/SCountryContext.h
struct SPolicyParams {
    // All current policies (taxes, regulations, spending, etc.)
    float m_CorporateTaxRate;
    float m_LaborTaxRate;
    float m_MinimumWage;
    // ... existing policy params ...
};

struct SCountryStats {
    // Basic country info (read-only, updated by modules)
    int32_t m_TotalPopulation;
    int32_t m_TotalWorkforce;
    float m_GDP;
    float m_HappinessIndex;
    // ... more stats ...
};

class CCountryContext {
private:
    SPolicyParams m_Policies;           // Current policies (read by modules)
    SCountryStats m_Stats;              // Country statistics (read by modules)

public:
    // Accessors for modules
    const SPolicyParams& GetPolicies() const { return m_Policies; }
    const SCountryStats& GetStats() const { return m_Stats; }

    // Called by government system when policies change
    void SetPolicies(const SPolicyParams& policies);

    // Called by modules to update stats
    void UpdatePopulation(int32_t delta);
    void UpdateGDP(float delta);
};
```

**Key Points:**
- `CCountryContext` lives in `CCountry` and is passed to all modules
- Modules read policies/context via `const&` accessors
- Government system writes policies, modules read them
- No need to pass context through function calls

---

### Layer 2: Module Interface (ECS-Inspired)

```cpp
// include/Country/ICountryModule.h
class ICountryModule {
public:
    virtual ~ICountryModule() = default;

    // Lifecycle
    virtual void Initialize(CCountryContext& context) = 0;
    virtual void Update(float gameDelta, const CCountryContext& context) = 0;
    virtual void Shutdown() = 0;

    // Module identification
    virtual std::string_view GetModuleName() const = 0;
    virtual EModuleType GetModuleType() const = 0;
};

enum class EModuleType {
    Economy,
    Population,
    Government,
    Resources,
    Laws,
    Education,
    Healthcare
    // ... more as needed
};
```

**Design Pattern:**
- All modules implement `ICountryModule`
- `Initialize()` receives mutable context (for setup)
- `Update()` receives const context (read-only policies/stats)
- Modules update their internal state, context is read-only during simulation

---

### Layer 3: Swappable Economy Systems

```cpp
// include/Country/Economy/IEconomySystem.h
class IEconomySystem : public ICountryModule {
public:
    virtual ~IEconomySystem() = default;

    // Economy-specific interface
    virtual void ApplyPolicy(const SPolicyParams& policy) = 0;
    virtual float GetGDP() const = 0;
    virtual float GetUnemployment() const = 0;

    // Factory method for creating different economy types
    static std::unique_ptr<IEconomySystem> Create(EEconomyType type);
};

enum class EEconomyType {
    Capitalist,     // Market-driven, private companies
    Socialist,      // State-owned enterprises, central planning
    Mixed           // Combination of both
};

// Implementation: Capitalist (already exists - CEconomyManager)
class CEconomyCapitalist : public IEconomySystem {
private:
    std::unique_ptr<CEconomyManager> m_Manager;  // Reuse existing code

public:
    void Initialize(CCountryContext& context) override;
    void Update(float gameDelta, const CCountryContext& context) override;

    // Capitalist-specific: Market-driven, private companies
    void ApplyPolicy(const SPolicyParams& policy) override;
};

// Implementation: Socialist (NEW - to be implemented later)
class CEconomySocialist : public IEconomySystem {
private:
    // State-owned enterprises, central planning
    std::vector<CStateEnterprise> m_StateEnterprises;
    CFiveYearPlan* m_CurrentPlan;

public:
    void Initialize(CCountryContext& context) override;
    void Update(float gameDelta, const CCountryContext& context) override;

    // Socialist-specific: Central planning, state ownership
    void SetFiveYearPlan(const CFiveYearPlan& plan);
};
```

**Switching Mechanism:**
```cpp
// In CCountry
void SetEconomyType(EEconomyType newType) {
    m_EconomyModule->Shutdown();
    m_EconomyModule = IEconomySystem::Create(newType);
    m_EconomyModule->Initialize(m_Context);
}
```

---

### Layer 4: Government System (Decision Making)

```cpp
// include/Country/Government/IGovernmentSystem.h
class IGovernmentSystem : public ICountryModule {
public:
    virtual ~IGovernmentSystem() = default;

    // Government-specific interface
    virtual void ProposePolicy(const SPolicyParams& policy) = 0;
    virtual bool IsPolicyApproved(const SPolicyParams& policy) const = 0;
    virtual void UpdatePolicies(float gameDelta) = 0;

    // Factory
    static std::unique_ptr<IGovernmentSystem> Create(EGovernmentType type);
};

enum class EGovernmentType {
    ParliamentaryRepublic,    // Congress deliberates & votes
    PresidentialRepublic,     // President proposes, Congress approves
    Authoritarian,           // Dictator decrees
    ConstitutionalMonarchy    // Monarch + Parliament
};

// Implementation: Parliamentary Republic
class CGovernmentParliamentary : public IGovernmentSystem {
private:
    struct CongressMember {
        std::string m_Name;
        EParty m_Party;
        float m_Ideology;  // -1.0 (left) to +1.0 (right)
        float m_Corruption;
    };

    std::vector<CongressMember> m_Congress;
    std::queue<SPolicyParams> m_ProposedPolicies;

public:
    void ProposePolicy(const SPolicyParams& policy) override;
    bool IsPolicyApproved(const SPolicyParams& policy) const override;
    void UpdatePolicies(float gameDelta) override;

private:
    // Simulate congressional deliberation
    float CalculateSupport(const SPolicyParams& policy) const;
    bool Vote(float supportRequired);
};

// Implementation: Authoritarian
class CGovernmentAuthoritarian : public IGovernmentSystem {
private:
    struct Dictator {
        std::string m_Name;
        float m_Power;  // 0-1
        float m_Stability;
        float m_ArmySupport;
    };

    Dictator m_Dictator;

public:
    void ProposePolicy(const SPolicyParams& policy) override;
    bool IsPolicyApproved(const SPolicyParams& policy) const override;

    // Dictator always approves (unless coup risk)
    bool CheckCoupRisk() const;
};
```

**Key Separation:**
- `IGovernmentSystem` decides WHICH policies become active
- `SPolicyParams` contains the policy data (tax rates, etc.)
- Economy/Population modules react to policies (read from context)

---

### Layer 5: Country Container

```cpp
// include/Country/CCountry.h
class CCountry {
private:
    std::string m_Name;

    // Shared context (accessed by all modules)
    CCountryContext m_Context;

    // Swappable modules
    std::unique_ptr<IEconomySystem> m_EconomyModule;
    std::unique_ptr<IGovernmentSystem> m_GovernmentModule;
    std::unique_ptr<IPopulationModule> m_PopulationModule;
    std::unique_ptr<IResourcesModule> m_ResourcesModule;
    std::unique_ptr<ILawsModule> m_LawsModule;

public:
    CCountry(const std::string& name);
    ~CCountry() = default;

    // Lifecycle
    void Initialize();
    void Update(float gameDelta);
    void Shutdown();

    // Module management
    void SetEconomyType(EEconomyType type);
    void SetGovernmentType(EGovernmentType type);

    // Accessors
    IEconomySystem* GetEconomy() { return m_EconomyModule.get(); }
    IGovernmentSystem* GetGovernment() { return m_GovernmentModule.get(); }
    CCountryContext& GetContext() { return m_Context; }
};
```

---

## Integration with Existing Code

### Step 1: Refactor Current Economy

**Current:**
```cpp
// include/politic_game.h
class CPoliticalGame {
    std::unique_ptr<CEconomyManager> m_EconomyManager;
};
```

**After:**
```cpp
// include/politic_game.h
class CPoliticalGame {
    std::unique_ptr<CCountry> m_Country;  // Owns all modules
};

// Access economy via country:
m_Country->GetEconomy()->GetGDP();
m_Country->GetContext().GetPolicies().m_CorporateTaxRate;
```

### Step 2: Add Country to Game Loop

```cpp
// src/politic_game.cpp
void CPoliticalGame::Update(float deltaTime) {
    // Time system
    m_TimeManager->Update(deltaTime);
    float gameDelta = m_TimeManager->GetDeltaGameTime();

    // Country (contains all modules)
    m_Country->Update(gameDelta);

    // World (rendering)
    m_World->Update(gameDelta);
}
```

---

## Implementation Phases (Updated Based on User Decisions)

### Phase 1: Foundation - Module System (Priority: HIGH)
**Goal:** Create the module system and country container

**Files to Create:**
1. `include/Country/SCountryContext.h` - Shared context (economic policies + population stats)
2. `include/Country/ICountryModule.h` - Module interface
3. `include/Country/CCountry.h` - Country container
4. `src/Country/CCountry.cpp` - Country lifecycle

**Implementation:**
- `CCountryContext` contains:
  - `SPolicyParams` (economic policies only - taxes, regulations, tariffs)
  - `SCountryStats` (population, workforce, GDP, unemployment)
- `ICountryModule` interface with `Initialize()`, `Update()`, `Shutdown()`
- `CCountry` owns all modules, provides context access
- ~200 lines of code

**Integration:**
- Move `SPolicyParams` from `Economy` namespace to `Country` namespace
- Refactor `CEconomyManager` to read policies from context instead of owning them
- Update `CPoliticalGame` to own `CCountry` instead of `CEconomyManager`

---

### Phase 2: Population Module (Priority: HIGH)
**Goal:** Add population system for employment simulation

**Files to Create:**
1. `include/Country/Population/IPopulationModule.h` - Population interface
2. `include/Country/Population/SPopulationState.h` - Population state struct
3. `include/Country/Population/CPopulationModule.h` - Population module
4. `src/Country/Population/CPopulationModule.cpp` (~250 lines)

**Implementation:**
```cpp
struct SPopulationState {
    int32_t m_TotalPopulation;
    int32_t m_Workforce;           // Working-age population
    int32_t m_Employed;            // Currently employed
    int32_t m_Unemployed;          // Looking for work
    float m_UnemploymentRate;      // Unemployed / Workforce
    float m_BirthRate;             // Births per 1000
    float m_DeathRate;             // Deaths per 1000
    float m_Happiness;             // 0-1
};

class CPopulationModule : public ICountryModule {
private:
    SPopulationState m_State;

public:
    void Update(float gameDelta, const CCountryContext& context) override;

    // Called by economy module when companies hire/fire
    void UpdateEmployment(int32_t employedDelta);
};
```

**Population Mechanics:**
- Initial population: 10M citizens, 5M workforce
- `UpdateEmployment()` called by economy when companies change hiring
- Birth/death rates based on happiness and economic conditions
- Unemployment affects happiness (high unemployment = low happiness)
- Updates `m_Context.m_Stats.m_Employed` for economy to read

**Integration with Economy:**
```cpp
// In CEconomyManager
void CEconomyManager::SimulateAllCompanies() {
    int32_t previousEmployment = m_MacroState.m_TotalEmployment;

    // ... simulate companies ...

    int32_t employmentDelta = m_MacroState.m_TotalEmployment - previousEmployment;
    m_Context.GetPopulationModule()->UpdateEmployment(employmentDelta);
}
```

---

### Phase 3: Government System - Manual Trigger (Priority: HIGH)
**Goal:** Implement government with manual policy trigger + deliberation

**Files to Create:**
1. `include/Country/Government/IGovernmentSystem.h` - Government interface
2. `include/Country/Government/SGovernmentTypes.h` - Government enums
3. `include/Country/Government/SPendingProposal.h` - Policy proposal struct
4. `include/Country/Government/CGovernmentParliamentary.h`
5. `include/Country/Government/CGovernmentAuthoritarian.h`
6. `src/Country/Government/CGovernmentParliamentary.cpp` (~250 lines)
7. `src/Country/Government/CGovernmentAuthoritarian.cpp` (~150 lines)

**Implementation:**
```cpp
struct SPendingProposal {
    SPolicyParams m_ProposedPolicies;
    float m_ProposalDate;          // When proposed
    float m_DeliberationEndDate;   // When voting happens
    bool m_IsApproved;
    int32_t m_SponsorCongressMemberID;
};

enum class EGovernmentType {
    ParliamentaryRepublic,    // Congress votes (1-3 month deliberation)
    PresidentialRepublic,     // President proposes + Congress approves
    Authoritarian,           // Dictator decrees immediately
    ConstitutionalMonarchy    // Monarch + Parliament
};

class CGovernmentParliamentary : public IGovernmentSystem {
private:
    struct CongressMember {
        std::string m_Name;
        EParty m_Party;
        float m_Ideology;        // -1.0 (left) to +1.0 (right)
        float m_Corruption;      // 0-1
    };

    std::vector<CongressMember> m_Congress;  // 100-300 members
    std::queue<SPendingProposal> m_Proposals;

public:
    // Called by ImGui button
    void ProposePolicy(const SPolicyParams& policy) override;

    // Update processes proposals through deliberation → voting
    void Update(float gameDelta, const CCountryContext& context) override;

private:
    float CalculateSupport(const SPolicyParams& policy) const;
    bool Vote(float supportRequired);
};
```

**Deliberation Mechanics:**
- **Parliamentary:**
  1. User clicks "Propose Policy" in ImGui
  2. Proposal enters queue with 1-3 month deliberation period
  3. After deliberation, congress votes based on ideology alignment
  4. If approved → `m_Context.SetPolicies(newPolicies)`
  5. Economy reacts in next `Update()` tick

- **Authoritarian:**
  1. User clicks "Decree Policy" in ImGui
  2. Immediate approval (dictator decrees)
  3. Small chance of coup if policies are extreme
  4. `m_Context.SetPolicies(newPolicies)` immediately

**Congress Voting Logic:**
```cpp
bool CGovernmentParliamentary::Vote(float supportRequired) {
    float totalSupport = 0.0f;

    for (const auto& member : m_Congress) {
        // Calculate ideology alignment with proposed policy
        float alignment = CalculateIdeologyAlignment(member, m_ProposedPolicies);

        // Corruption makes members susceptible to lobbying
        if (member.m_Corruption > 0.5f && random() < 0.3f) {
            alignment = 1.0f;  // Bought vote
        }

        totalSupport += alignment;
    }

    float supportRatio = totalSupport / m_Congress.size();
    return supportRatio >= supportRequired;
}
```

---

### Phase 4: Integration (Priority: HIGH)
**Goal:** Connect all systems

**Files to Modify:**
1. `src/Economy/CEconomyManager.cpp` - Read policies from context, update population
2. `src/CPopulationModule.cpp` - Update employment based on economy
3. `src/politic_game.cpp` - Add ImGui windows for government

**ImGui Windows:**
1. **Government Overview:**
   - Government type (Parliamentary/Authoritarian/etc.)
   - Congress composition (parties, ideology distribution)
   - List of active proposals (with status: deliberating/voting/approved/rejected)

2. **Proposal Window:**
   - Sliders for all economic policies
   - "Propose Policy" button (or "Decree" for authoritarian)
   - Shows deliberation time remaining

3. **Population Stats:**
   - Total population, workforce
   - Employed/unemployed counts
   - Unemployment rate
   - Happiness index

---

### Phase 5: System Switching with Transition (Priority: MEDIUM)
**Goal:** Implement gradual economy/government type transitions

**Implementation:**
```cpp
class CCountry {
private:
    enum class ETransitionState {
        None,
        TransitioningEconomy,
        TransitioningGovernment
    };

    ETransitionState m_TransitionState;
    float m_TransitionProgress;    // 0.0 to 1.0
    EEconomyType m_TargetEconomyType;

public:
    void SetEconomyType(EEconomyType newType) {
        m_TransitionState = ETransitionState::TransitioningEconomy;
        m_TransitionProgress = 0.0f;
        m_TargetEconomyType = newType;
    }

    void Update(float gameDelta) {
        if (m_TransitionState != ETransitionState::None) {
            UpdateTransition(gameDelta);
        }

        // Normal module updates
        m_EconomyModule->Update(gameDelta, m_Context);
        // ...
    }

private:
    void UpdateTransition(float gameDelta) {
        // Transition takes 6-12 game months
        float transitionDuration = 360.0f;  // 12 months in days
        m_TransitionProgress += gameDelta / transitionDuration;

        if (m_TransitionProgress >= 1.0f) {
            CompleteTransition();
        }
    }

    void CompleteTransition() {
        if (m_TransitionState == ETransitionState::TransitioningEconomy) {
            m_EconomyModule->Shutdown();
            m_EconomyModule = IEconomySystem::Create(m_TargetEconomyType);
            m_EconomyModule->Initialize(m_Context);
        }

        m_TransitionState = ETransitionState::None;
    }
};
```

**Transition Mechanics (Capitalist → Socialist):**
- Months 1-3: Government nationalizes 10% of largest companies/month
- Months 4-6: Create state enterprises, transfer employees
- Months 7-12: Full socialist system active
- Employment preserved throughout transition
- GDP may fluctuate during transition period

---

### Phase 6: Socialist Economy (Priority: LOW - Future)
**Goal:** Demonstrate swappable economy system

**Deferred until Phase 1-5 are working**

---

## Data Flow Examples

### Example 1: Congress Raises Taxes

```
1. ImGui UI: User clicks "Propose Tax Increase"
   ↓
2. CGovernmentParliamentary::ProposePolicy()
   - Creates SPolicyParams with m_CorporateTaxRate = 30.0f
   - Adds to m_ProposedPolicies queue
   ↓
3. CGovernmentParliamentary::UpdatePolicies() (each month)
   - Pops policy from queue
   - CalculatesSupport(): Checks if congress approves
   - Vote(): If 50%+ support → approved
   ↓
4. CCountryContext::SetPolicies()
   - m_Policies.m_CorporateTaxRate = 30.0f
   ↓
5. CEconomyCapitalist::Update() (next tick)
   - Reads m_Context.GetPolicies().m_CorporateTaxRate
   - Companies calculate costs with new tax rate
   - Profits decrease, hiring slows
   ↓
6. ImGui UI: Shows higher unemployment, lower profits
```

### Example 2: Dictator Nationalizes Industry

```
1. User changes government: Parliamentary → Authoritarian
   ↓
2. CCountry::SetGovernmentType(Authoritarian)
   - m_GovernmentModule->Shutdown()
   - m_GovernmentModule = IGovernmentSystem::Create(Authoritarian)
   - m_GovernmentModule->Initialize(m_Context)
   ↓
3. User changes economy: Capitalist → Socialist
   ↓
4. CCountry::SetEconomyType(Socialist)
   - Destroys private companies
   - Creates state-owned enterprises
   ↓
5. CEconomySocialist::Update()
   - Central planning sets production
   - No market competition
   - Employment guaranteed
```

---

## Policy Data Structure

### Complete Policy Set

```cpp
// include/Country/SCountryContext.h
struct SPolicyParams {
    // === Economic Policies ===
    float m_CorporateTaxRate;           // 0-100%
    float m_LaborTaxRate;               // 0-100%
    float m_MinimumWage;                // $/hour
    float m_LaborRegulationBurden;      // 0-1
    float m_EnvironmentalComplianceCost;// 0-1
    bool m_StrictEnvironmentalPolicy;
    float m_SubsidyRate;                // % of costs subsidized
    bool m_SubsidiesEnabled;
    float m_TariffRate;                 // 0-100%
    float m_TradeAgreementLevel;        // 0-1 (openness)

    // === Social Policies ===
    float m_EducationSpending;          // % of GDP
    float m_HealthcareSpending;         // % of GDP
    float m_SecuritySpending;           // % of GDP
    float m_InfrastructureSpending;     // % of GDP
    bool m_UniversalHealthcare;
    bool m_FreeEducation;

    // === Political Policies ===
    bool m_FreeElections;               // Can vote?
    float m_PressFreedom;               // 0-1
    float m_DemonstrationsAllowed;      // 0-1
    bool m_MartialLaw;                  // Emergency powers

    // === Legal System ===
    float m_PropertyRightsProtection;   // 0-1
    float m_ContractEnforcement;        // 0-1
    bool m_ConstitutionalCourt;

    // === Welfare ===
    float m_UnemploymentBenefit;        // % of previous wage
    float m_PensionSpending;            // % of GDP
    bool m_UniversalBasicIncome;
    float m_UBIAmount;                  // $/month

    // Migration
    bool m_OpenBorders;
    float m_ImmigrationQuota;           // people/year
    bool m_AsyllumGranted;
};
```

---

## Code Quality Considerations

### 1. Avoid Circular Dependencies
```
✅ GOOD:
  CCountryContext (no dependencies on modules)
  ↓
  IEconomySystem (depends on CCountryContext)
  IGovernmentSystem (depends on CCountryContext)

❌ BAD:
  IEconomySystem → IGovernmentSystem
  IGovernmentSystem → IEconomySystem
```

### 2. Const Correctness
```cpp
// Modules only READ context during update
void Update(float gameDelta, const CCountryContext& context);

// Government WRITES policies (non-const)
void SetPolicies(const SPolicyParams& policies);
```

### 3. Factory Pattern for Swappable Systems
```cpp
// Clean switching mechanism
auto economy = IEconomySystem::Create(EEconomyType::Capitalist);
auto government = IGovernmentSystem::Create(EGovernmentType::Parliamentary);
```

### 4. Minimal Refactoring of Existing Code
- `CEconomyManager` becomes `CEconomyCapitalist`
- Most economy code stays the same
- Only policy access changes: `m_PolicyParams` → `m_Context.GetPolicies()`

---

## Testing Strategy

### Unit Tests (Future)
1. Test module switching without crashing
2. Test policy changes propagate to economy
3. Test different government types have different approval rates

### Integration Tests
1. Create country with parliamentary government
2. Propose tax increase via ImGui
3. Verify congress approves/rejects based on ideology
4. Verify economy reacts to new tax rate

### Manual Testing
1. Switch from Capitalist → Socialist economy
2. Observe private companies destroyed, state enterprises created
3. Switch from Parliamentary → Authoritarian
4. Observe voting disabled, dictator decrees immediately

---

## Success Criteria

### Phase 1-3 (MVP)
- [x] `CCountry` class owns modules
- [x] `CCountryContext` provides shared state
- [x] Existing economy refactored to use context
- [x] Government system can change policies
- [x] Economy reacts to policy changes
- [x] Can switch government types at runtime
- [x] ImGui shows both government + economy

### Phase 4-5 (Full System)
- [x] Population module exists
- [x] Multiple economy types (Capitalist/Socialist)
- [x] All government types implemented
- [x] Laws module exists
- [x] No crashes when switching systems

---

## Files to Create/Modify (Updated)

### Phase 1: Foundation (4 new files, 3 modified)
```
include/Country/
├── SCountryContext.h              (100 lines) - Economic policies + population stats
├── ICountryModule.h               (50 lines)  - Module interface
├── CCountry.h                     (80 lines)  - Country container
└── ECountryTypes.h                (30 lines)  - Country enums

src/Country/
└── CCountry.cpp                   (150 lines) - Country lifecycle

include/Economy/
└── SPolicyParams.h → MOVE to include/Country/SCountryContext.h

include/politic_game.h
├── REMOVE: m_EconomyManager
├── ADD: m_Country

src/Economy/CEconomyManager.cpp
├── REFACTOR: Read policies from context instead of owning them

src/politic_game.cpp
├── REFACTOR: Initialize() to create CCountry
└── REFACTOR: Update() to call m_Country->Update()
```

### Phase 2: Population Module (4 new files)
```
include/Country/Population/
├── IPopulationModule.h            (40 lines)  - Population interface
├── SPopulationState.h             (60 lines)  - Population state struct
└── CPopulationModule.h            (50 lines)  - Population module

src/Country/Population/
└── CPopulationModule.cpp          (250 lines) - Population simulation
```

### Phase 3: Government System (7 new files)
```
include/Country/Government/
├── IGovernmentSystem.h            (60 lines)  - Government interface
├── SGovernmentTypes.h             (40 lines)  - Government enums
├── SPendingProposal.h             (50 lines)  - Policy proposal struct
├── CGovernmentParliamentary.h     (60 lines)
└── CGovernmentAuthoritarian.h    (50 lines)

src/Country/Government/
├── CGovernmentParliamentary.cpp   (250 lines) - Congress simulation
└── CGovernmentAuthoritarian.cpp   (150 lines) - Dictator simulation
```

### Phase 4: Integration (2 modified files)
```
src/Economy/CEconomyManager.cpp
├ MODIFY: SimulateAllCompanies() to update population employment

src/politic_game.cpp
├ ADD: ImGui windows for Government Overview, Proposal, Population
```

### Phase 5: System Switching (2 modified)
```
include/Country/CCountry.h
├ ADD: Transition state, progress, target type

src/Country/CCountry.cpp
├ ADD: UpdateTransition(), CompleteTransition()
```

---

## Total Lines of Code Estimate

### Phase 1: Foundation
- **New:** 380 lines
- **Modified:** ~100 lines (refactor)

### Phase 2: Population
- **New:** 400 lines

### Phase 3: Government
- **New:** 560 lines

### Phase 4: Integration
- **Modified:** ~150 lines (ImGui + integration)

### Phase 5: System Switching
- **Modified:** ~80 lines

**Total:**
- **New Code:** ~1,340 lines
- **Modified Code:** ~330 lines
- **Total:** ~1,670 lines

---

## Estimated Complexity

### Phase 1 (Foundation)
- **Lines of Code:** ~350
- **New Files:** 3
- **Modified Files:** 2
- **Risk:** Low (pure addition, minimal refactoring)
- **Time:** Short iteration

### Phase 2 (Government)
- **Lines of Code:** ~350
- **New Files:** 6
- **Risk:** Low (independent from economy)
- **Time:** Short iteration

### Phase 3 (Integration)
- **Lines of Code:** ~50
- **Modified Files:** 2
- **Risk:** Medium (refactor existing code)
- **Time:** Short iteration

**Total for Phase 1-3:** ~750 lines, 9 new files, 4 modified files

---

## User Decisions (via Q&A)

### 1. Policy Change Frequency: Manual Trigger ✅
- **Decision:** Trigger manual (user button + deliberation time)
- **Implementation:**
  - ImGui button "Propose Policy Change"
  - Government system deliberates (time varies by type)
  - Parliamentary: 1-3 months deliberation + voting
  - Authoritarian: Immediate (dictator decrees)
  - Policy takes effect after approval/deliberation

### 2. Congressional Detail: Medium (Individual Members) ✅
- **Decision:** Medium detail (individual members, parties, corruption)
- **Implementation:**
  - 100-300 congress members with individual attributes
  - Party affiliation + ideology (-1.0 left to +1.0 right)
  - Corruption level affects voting susceptibility
  - ~250 lines of code
  - Matches company simulation detail level

### 3. System Switching: Gradual Transition ✅
- **Decision:** Transition period (gradual conversion)
- **Implementation:**
  - When switching economy/government types:
    - Preserve macro stats (GDP, population, unemployment)
    - Destroy old system-specific entities (companies/congress)
    - Create new entities over 6-12 game months
    - Transition phase with mixed systems
  - Example: Capitalist → Socialist:
    - Month 1-6: State nationalizes companies gradually
    - Month 7-12: State enterprises established
    - Employment preserved during transition

### 4. Policy Scope: Economic + Population Module ✅
- **Decision:** Economic policies (existing) + Population module for employment simulation
- **Implementation:**
  - Phase 1-3: Focus on economic policies only (taxes, regulations, tariffs)
  - Phase 4: Add population module to track:
    - Total population
    - Workforce (employed + unemployed)
    - Employment rate effects from company hiring/firing
    - Population growth/death based on economic conditions
  - Defer social/political policies (elections, press freedom) to later phases

---

## Recommended Implementation Order

### Iteration 1: Foundation + Population (Phase 1-2)
**Goal:** Basic country system with population tracking

1. Create `CCountry` with `CCountryContext`
2. Move `SPolicyParams` from Economy to Country namespace
3. Refactor `CEconomyManager` to read policies from context
4. Implement `CPopulationModule`
5. Connect economy employment changes to population

**Outcome:** Game runs with country system, population tracks employment

### Iteration 2: Government (Phase 3)
**Goal:** Manual policy trigger with deliberation

1. Implement `CGovernmentParliamentary` with congress members
2. Implement `CGovernmentAuthoritarian` with dictator
3. Add ImGui "Propose Policy" button
4. Add proposal queue with deliberation time
5. Connect government to context (writes policies)

**Outcome:** User can propose policies, congress deliberates/votes, economy reacts

### Iteration 3: Integration + Polish (Phase 4)
**Goal:** Complete UI and feedback loops

1. Add Government Overview ImGui window
2. Add Population Stats ImGui window
3. Show proposal status (deliberating/voting/approved/rejected)
4. Show congress composition (parties, ideology)
5. Test complete loop: propose → deliberate → vote → implement → economy reacts

**Outcome:** Fully functional government-economy loop

### Iteration 4: System Switching (Phase 5)
**Goal:** Demonstrate swappable systems

1. Add transition state to `CCountry`
2. Implement gradual transition mechanics
3. Add ImGui buttons to switch government/economy types
4. Test transitions (Parliamentary → Authoritarian)
5. (Future) Implement socialist economy for full economy switching

**Outcome:** Can switch between government types at runtime

---

## Success Criteria (Updated)

### Iteration 1 (Foundation + Population)
- [x] `CCountry` owns `CEconomyManager` and `CPopulationModule`
- [x] `CCountryContext` provides shared policies + stats
- [x] Economy reads policies from context (not owned)
- [x] Population tracks employment from economy
- [x] Game compiles and runs without crashes
- [x] ImGui shows population stats (total, employed, unemployed, unemployment rate)

### Iteration 2 (Government)
- [x] Government module can propose policies
- [x] Congress has 100-300 members with ideology + corruption
- [x] Deliberation period (1-3 months) before voting
- [x] Voting based on ideology alignment
- [x] Authoritarian government approves immediately
- [x] Approved policies update context
- [x] Economy reacts to policy changes

### Iteration 3 (Integration)
- [x] ImGui Government Overview shows congress composition
- [x] ImGui Proposal window allows policy adjustments
- [x] Proposal queue shows status (deliberating/voting/approved/rejected)
- [x] ImGui Population shows employment trends
- [x] Complete loop works: propose → deliberate → vote → implement → economy reacts

### Iteration 4 (System Switching)
- [x] Can switch Parliamentary ↔ Authoritarian at runtime
- [x] Transition takes 6-12 game months
- [x] Congress/dictator destroyed and recreated during transition
- [x] Economy and population preserved during transition
- [x] No crashes during transition

---

## References

- **Existing Economy:** `/home/cucsijuan/Drives/Projects/Projects/C++/politic-sim/include/Economy/`
- **Existing Design:** `/home/cucsijuan/Drives/Projects/Projects/C++/politic-sim/docs/Company_Economy_Design.md`
- **SDL Engine Style:** `/home/cucsijuan/Drives/Projects/Projects/C++/politic-sim/vendor/SDL-Engine/documentation/coding_style_guide.md`
- **Time System:** `/home/cucsijuan/Drives/Projects/Projects/C++/politic-sim/include/Time/CTimeManager.h`

---

**End of Country System Architecture Plan**
