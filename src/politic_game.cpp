#include "politic_game.h"

#include <Engine/Core/Camera/CameraManager.h>
#include <Economy/ECompanyTypes.h>
#include <Economy/SCompanyState.h>
#include <Economy/SCompanyAttributes.h>
#include <Economy/CCompany.h>
#include <iostream>
#include <vector>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

namespace PoliticSim {

bool CPoliticalGame::Initialize() {
	std::cout << "Initializing Political Game..." << std::endl;

	// Get keyboard state pointer
	m_KeyboardState = SDL_GetKeyboardState(nullptr);

	// Create world with bounds
	SWorldBounds bounds(glm::vec3(-2000, -2000, -100), glm::vec3(2000, 2000, 100));
	m_World = std::make_unique<CWorld>("MainWorld", bounds);

	// Create free camera
	m_Camera = std::make_unique<CCameraFollowGO>(1024.0f, 768.0f);
	m_Camera->SetOrthographic(400.0f);
	m_Camera->SetPosition(0, 0, 10);
	m_Camera->SetupFreeCamera();
	m_Camera->SetFreeCameraSpeed(CAMERA_SPEED);

	// Set camera through manager
	CCameraManager::GetInstance().SetActiveCamera(m_Camera.get());

	// Initialize time manager
	m_TimeManager = std::make_unique<CTimeManager>();
	m_TimeManager->Initialize();
	std::cout << "Time Manager initialized (Month-based granularity)" << std::endl;

	// Initialize economy manager
	m_EconomyManager = std::make_unique<CEconomyManager>();
	m_EconomyManager->Initialize();
	std::cout << "Economy Manager initialized" << std::endl;

	std::cout << "Political Game initialized successfully!" << std::endl;
	std::cout << "Controls: WASD to move camera" << std::endl;
	std::cout << "          SPACE: Pause/Resume" << std::endl;
	std::cout << "          1-5: Set speed level" << std::endl;
	std::cout << "          +/-: Increase/Decrease speed" << std::endl;
	return true;
}

void CPoliticalGame::HandleInput(const SDL_Event& event) {
	HandleDiscreteInput(event);
}

void CPoliticalGame::HandleDiscreteInput(const SDL_Event& event) {
	if (event.type == SDL_EVENT_KEY_DOWN) {
		switch (event.key.key) {
		case SDLK_ESCAPE:
			std::cout << "Exit requested" << std::endl;
			break;

		case SDLK_P:
			{
				glm::vec2 cameraPos = m_Camera->GetPosition2D();
				std::cout << "Camera position: (" << cameraPos.x << ", " << cameraPos.y << ")" << std::endl;
			}
			break;

		// Time controls
		case SDLK_SPACE:
			if (m_TimeManager)
			{
				m_TimeManager->TogglePause();
				std::cout << "Time " << (m_TimeManager->IsPaused() ? "paused" : "resumed") << std::endl;
			}
			break;

		case SDLK_1:
			if (m_TimeManager)
			{
				m_TimeManager->SetSpeedByIndex(1); // Slow (1x)
				std::cout << "Speed set to: Slow (1x)" << std::endl;
			}
			break;

		case SDLK_2:
			if (m_TimeManager)
			{
				m_TimeManager->SetSpeedByIndex(2); // Normal (2x)
				std::cout << "Speed set to: Normal (2x)" << std::endl;
			}
			break;

		case SDLK_3:
			if (m_TimeManager)
			{
				m_TimeManager->SetSpeedByIndex(3); // Fast (5x)
				std::cout << "Speed set to: Fast (5x)" << std::endl;
			}
			break;

		case SDLK_4:
			if (m_TimeManager)
			{
				m_TimeManager->SetSpeedByIndex(4); // Very Fast (10x)
				std::cout << "Speed set to: Very Fast (10x)" << std::endl;
			}
			break;

		case SDLK_5:
			if (m_TimeManager)
			{
				m_TimeManager->SetSpeedByIndex(5); // Maximum (30x)
				std::cout << "Speed set to: Maximum (30x)" << std::endl;
			}
			break;

		case SDLK_PLUS:
		case SDLK_EQUALS:
		case SDLK_KP_PLUS:
			if (m_TimeManager)
			{
				m_TimeManager->IncreaseSpeed();
				std::cout << "Speed increased to: " << m_TimeManager->GetSpeedName() << std::endl;
			}
			break;

		case SDLK_MINUS:
		case SDLK_KP_MINUS:
			if (m_TimeManager)
			{
				m_TimeManager->DecreaseSpeed();
				std::cout << "Speed decreased to: " << m_TimeManager->GetSpeedName() << std::endl;
			}
			break;
		}
	}
	else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (event.button.button == SDL_BUTTON_LEFT) {
			std::cout << "Mouse clicked at: (" << event.button.x << ", " << event.button.y << ")" << std::endl;
		}
	}
}

void CPoliticalGame::HandleContinuousInput(float deltaTime) {
	UpdateCameraMovement(deltaTime);
}

void CPoliticalGame::UpdateCameraMovement(float deltaTime) {
	if (!m_Camera || !m_KeyboardState) return;

	// Calculate camera movement vector
	glm::vec2 cameraMovement(0.0f);

	if (m_KeyboardState[SDL_SCANCODE_W] || m_KeyboardState[SDL_SCANCODE_UP]) {
		cameraMovement.y += 1.0f;
	}
	if (m_KeyboardState[SDL_SCANCODE_S] || m_KeyboardState[SDL_SCANCODE_DOWN]) {
		cameraMovement.y -= 1.0f;
	}
	if (m_KeyboardState[SDL_SCANCODE_A] || m_KeyboardState[SDL_SCANCODE_LEFT]) {
		cameraMovement.x -= 1.0f;
	}
	if (m_KeyboardState[SDL_SCANCODE_D] || m_KeyboardState[SDL_SCANCODE_RIGHT]) {
		cameraMovement.x += 1.0f;
	}

	// Normalize and apply to camera
	if (glm::length(cameraMovement) > 0.0f) {
		cameraMovement = glm::normalize(cameraMovement);
		glm::vec2 velocity = cameraMovement * CAMERA_SPEED;
		m_Camera->SetFreeCameraVelocity(velocity);
	} else {
		m_Camera->SetFreeCameraVelocity(glm::vec2(0.0f));
	}
}

void CPoliticalGame::Update(float deltaTime) {
	// Update time manager FIRST (converts real delta to game delta)
	if (m_TimeManager)
	{
		m_TimeManager->Update(deltaTime);
	}

	// Get game time delta for game systems
	float gameDelta = m_TimeManager ? m_TimeManager->GetDeltaGameTime() : deltaTime;

	// Camera uses real-time for smooth movement
	HandleContinuousInput(deltaTime);

	// Economy manager uses game time for simulation
	if (m_EconomyManager)
	{
		m_EconomyManager->Update(gameDelta);
	}

	// World uses game time for simulation
	if (m_World)
	{
		m_World->Update(gameDelta);
	}

	// Camera always uses real-time for smooth visuals
	if (m_Camera)
	{
		m_Camera->Update(deltaTime);
	}
}

void CPoliticalGame::Render(CRenderer& renderer) {
	if (m_World) {
		m_World->Render(renderer);
	}

	// New ImGui frame
	ImGui_ImplSDLRenderer3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// Game UI
	ImGui::Begin("Politic Sim");
	ImGui::Text("Political Simulation Game");
	ImGui::Separator();
	ImGui::Text("Controls:");
	ImGui::Text("WASD / Arrow Keys: Move camera");
	ImGui::Text("P: Print camera position");
	ImGui::Text("SPACE: Pause/Resume time");
	ImGui::Text("1-5: Set speed level");
	ImGui::Text("+/-: Increase/Decrease speed");
	ImGui::Text("ESC: Exit");
	ImGui::End();

	// Time Controls UI
	if (m_TimeManager)
	{
		ImGui::Begin("Time Controls");

		// Display current time
		const SGameTime& gameTime = m_TimeManager->GetCurrentTime();
		ImGui::Text("Current Date: Year %d, Month %d", gameTime.years + 1, gameTime.months + 1);

		ImGui::Separator();

		// Speed controls
		const std::vector<STimeSpeedConfig>& speeds = m_TimeManager->GetAllSpeeds();
		size_t currentSpeedIndex = m_TimeManager->GetCurrentSpeedIndex();

		ImGui::Text("Game Speed:");

		for (size_t i = 1; i < speeds.size(); ++i) // Skip Paused (index 0)
		{
			const auto& speed = speeds[i];

			// Highlight active speed
			if (currentSpeedIndex == i)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));
			}

			std::string label = speed.name + " (" + speed.displayLabel + ")";

			if (ImGui::Button(label.c_str()))
			{
				m_TimeManager->SetSpeedByIndex(i);
			}

			if (currentSpeedIndex == i)
			{
				ImGui::PopStyleColor();
			}

			if (i < speeds.size() - 1)
			{
				ImGui::SameLine();
			}
		}

		ImGui::Separator();

		// Pause/Resume button
		if (m_TimeManager->IsPaused())
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
			if (ImGui::Button("Resume [SPACE]", ImVec2(120, 0)))
			{
				m_TimeManager->TogglePause();
			}
			ImGui::PopStyleColor();
		}
		else
		{
			if (ImGui::Button("Pause [SPACE]", ImVec2(120, 0)))
			{
				m_TimeManager->TogglePause();
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Slower [-]", ImVec2(100, 0)))
		{
			m_TimeManager->DecreaseSpeed();
		}

		ImGui::SameLine();

		if (ImGui::Button("Faster [+]", ImVec2(100, 0)))
		{
			m_TimeManager->IncreaseSpeed();
		}

		ImGui::Separator();

		// Statistics
		ImGui::Text("Statistics:");
		ImGui::Text("Real-time played: %.1f seconds", m_TimeManager->GetTotalRealTime());
		float gameDays = m_TimeManager->GetElapsedGameTime() / static_cast<float>(CTimeUnits::SECONDS_PER_DAY);
		ImGui::Text("Game-time elapsed: %.1f days", gameDays);
		ImGui::Text("Average FPS: %.1f", m_TimeManager->GetAverageFPS());
		ImGui::Text("Current multiplier: %.1fx", m_TimeManager->GetTimeMultiplier());

		ImGui::End();
	}

	// Policy Parameters UI
	if (m_EconomyManager)
	{
		ImGui::Begin("Policy Parameters");

		SPolicyParams& policy = m_EconomyManager->GetPolicyParams();

		ImGui::Text("Tax Policy");
		ImGui::SliderFloat("Corporate Tax Rate", &policy.m_CorporateTaxRate, 0.0f, 50.0f, "%.1f%%");
		ImGui::SliderFloat("Labor Tax Rate", &policy.m_LaborTaxRate, 0.0f, 30.0f, "%.1f%%");

		ImGui::Separator();

		ImGui::Text("Labor Regulations");
		ImGui::SliderFloat("Minimum Wage", &policy.m_MinimumWage, 0.0f, 30.0f, "$%.2f/hr");
		ImGui::SliderFloat("Labor Regulation Burden", &policy.m_LaborRegulationBurden, 0.0f, 1.0f, "%.2f");

		ImGui::Separator();

		ImGui::Text("Environmental Policy");
		ImGui::SliderFloat("Environmental Compliance Cost", &policy.m_EnvironmentalComplianceCost, 0.0f, 1.0f, "%.2f");
		ImGui::Checkbox("Strict Environmental Policy", &policy.m_StrictEnvironmentalPolicy);

		ImGui::Separator();

		ImGui::Text("Business Support");
		ImGui::Checkbox("Enable Subsidies", &policy.m_SubsidiesEnabled);
		if (policy.m_SubsidiesEnabled)
		{
			ImGui::SliderFloat("Subsidy Rate", &policy.m_SubsidyRate, 0.0f, 10.0f, "%.1f%%");
		}

		ImGui::Separator();

		ImGui::Text("Trade Policy");
		ImGui::SliderFloat("Tariff Rate", &policy.m_TariffRate, 0.0f, 50.0f, "%.1f%%");

		ImGui::End();
	}

	// Company Data UI
	if (m_EconomyManager)
	{
		ImGui::Begin("Company Data");

		// Aggregates
		const SMacroState& macro = m_EconomyManager->GetMacroState();

		ImGui::Text("Economy Overview");
		ImGui::Separator();
		ImGui::Text("Total Companies: %zu", m_EconomyManager->GetCompanyCount());
		ImGui::Text("Total Employment: %.0f", m_EconomyManager->GetTotalEmployment());
		ImGui::Text("Total GDP: $%.1fK", m_EconomyManager->GetTotalGDP());
		ImGui::Text("Average Profitability: $%.2fK", m_EconomyManager->GetAverageProfitability());
		ImGui::Text("Unemployment Rate: %.1f%%", m_EconomyManager->GetUnemploymentRate());
		ImGui::Text("Business Confidence: %.1f", macro.m_BusinessConfidence);
		ImGui::Text("Aggregate Demand: %.2f", macro.m_AggregateDemand);

		ImGui::Separator();
		ImGui::Separator();

		// Per-company table (scrollable)
		static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg |
		                                   ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter |
		                                   ImGuiTableFlags_SizingFixedFit;

		if (ImGui::BeginTable("Companies", 7, flags, ImVec2(0, 300)))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
			ImGui::TableSetupColumn("Sector", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 60.0f);
			ImGui::TableSetupColumn("Employees", ImGuiTableColumnFlags_WidthFixed, 70.0f);
			ImGui::TableSetupColumn("Profit", ImGuiTableColumnFlags_WidthFixed, 70.0f);
			ImGui::TableSetupColumn("Liquidity", ImGuiTableColumnFlags_WidthFixed, 70.0f);
			ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed, 70.0f);
			ImGui::TableHeadersRow();

			const auto& companies = m_EconomyManager->GetCompanies();
			for (const auto& company : companies)
			{
				const SCompanyState& state = company->GetState();
				const SCompanyAttributes& attrs = company->GetAttributes();

				ImGui::TableNextRow();
				ImGui::TableNextColumn();

				// Selectable row with highlight for selected company
				bool isSelected = (static_cast<int32_t>(company->GetID()) == m_SelectedCompanyID);
				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
				}

				std::string selectableLabel = "##" + std::to_string(company->GetID());
				if (ImGui::Selectable(selectableLabel.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap))
				{
					m_SelectedCompanyID = company->GetID();
				}

				if (isSelected)
				{
					ImGui::PopStyleColor();
				}

				// Display ID in the same column
				ImGui::SameLine(0, 0);
				ImGui::Text("%u", company->GetID());

				ImGui::TableNextColumn();
				const char* sector = nullptr;
				switch (attrs.m_Sector)
				{
					case ESector::Agriculture: sector = "Ag"; break;
					case ESector::Industry: sector = "Ind"; break;
					case ESector::Services: sector = "Svc"; break;
					case ESector::Technology: sector = "Tech"; break;
					case ESector::Retail: sector = "Ret"; break;
				}
				ImGui::Text("%s", sector);

				ImGui::TableNextColumn();
				const char* size = nullptr;
				switch (attrs.m_Size)
				{
					case ECompanySize::Micro: size = "Micro"; break;
					case ECompanySize::Small: size = "Small"; break;
					case ECompanySize::Medium: size = "Med"; break;
					case ECompanySize::Large: size = "Large"; break;
				}
				ImGui::Text("%s", size);

				ImGui::TableNextColumn();
				ImGui::Text("%d", state.m_Employees);

				ImGui::TableNextColumn();
				ImGui::Text("$%.1fK", state.m_Profitability);

				ImGui::TableNextColumn();
				ImGui::Text("$%.0fK", state.m_Liquidity);

				ImGui::TableNextColumn();
				const char* stateStr = nullptr;
				switch (state.m_State)
				{
					case ECompanyState::Growing: stateStr = "Grow"; break;
					case ECompanyState::Stable: stateStr = "Stable"; break;
					case ECompanyState::Declining: stateStr = "Decl"; break;
					case ECompanyState::Crisis: stateStr = "CRISIS"; break;
				}
				ImGui::Text("%s", stateStr);
			}

			ImGui::EndTable();
		}

		ImGui::End();
	}

	// Market Saturation Window
	if (m_EconomyManager)
	{
		ImGui::Begin("Market Saturation");

		const SMacroState& macro = m_EconomyManager->GetMacroState();

		ImGui::Text("Sector Saturation (higher = more competitive):");
		ImGui::Separator();
		ImGui::Text("Agriculture: %.1f%%", macro.m_SectorSaturation[0] * 100.0f);
		ImGui::Text("Industry: %.1f%%", macro.m_SectorSaturation[1] * 100.0f);
		ImGui::Text("Services: %.1f%%", macro.m_SectorSaturation[2] * 100.0f);
		ImGui::Text("Technology: %.1f%%", macro.m_SectorSaturation[3] * 100.0f);
		ImGui::Text("Retail: %.1f%%", macro.m_SectorSaturation[4] * 100.0f);

		ImGui::Separator();
		ImGui::Text("Import Competition:");
		ImGui::Separator();
		ImGui::Text("Agriculture: %.1f%%", macro.m_ImportCompetition[0] * 100.0f);
		ImGui::Text("Industry: %.1f%%", macro.m_ImportCompetition[1] * 100.0f);
		ImGui::Text("Services: %.1f%%", macro.m_ImportCompetition[2] * 100.0f);
		ImGui::Text("Technology: %.1f%%", macro.m_ImportCompetition[3] * 100.0f);
		ImGui::Text("Retail: %.1f%%", macro.m_ImportCompetition[4] * 100.0f);

		ImGui::Separator();
		ImGui::Text("Effects:");
		ImGui::Text("• High saturation = lower revenue, slower growth");
		ImGui::Text("• High import competition = domestic companies struggle");
		ImGui::Text("• Tariffs reduce import competition");
		ImGui::Text("• Large companies handle saturation better");

		ImGui::End();
	}

	// Company History Graph Window
	if (m_EconomyManager && m_SelectedCompanyID >= 0)
	{
		const CCompany* selectedCompany = nullptr;
		const auto& companies = m_EconomyManager->GetCompanies();

		// Find the selected company
		for (const auto& company : companies)
		{
			if (static_cast<int32_t>(company->GetID()) == m_SelectedCompanyID)
			{
				selectedCompany = company.get();
				break;
			}
		}

		if (selectedCompany)
		{
			ImGui::Begin("Company History");

			const SCompanyState& state = selectedCompany->GetState();
			const SCompanyAttributes& attrs = selectedCompany->GetAttributes();

			// Company info header
			ImGui::Text("Company ID: %u", selectedCompany->GetID());
			ImGui::SameLine();
			ImGui::Text("Sector: ");
			switch (attrs.m_Sector)
			{
				case ESector::Agriculture: ImGui::Text("Agriculture"); break;
				case ESector::Industry: ImGui::Text("Industry"); break;
				case ESector::Services: ImGui::Text("Services"); break;
				case ESector::Technology: ImGui::Text("Technology"); break;
				case ESector::Retail: ImGui::Text("Retail"); break;
			}
			ImGui::SameLine();
			ImGui::Text("Size: ");
			switch (attrs.m_Size)
			{
				case ECompanySize::Micro: ImGui::Text("Micro"); break;
				case ECompanySize::Small: ImGui::Text("Small"); break;
				case ECompanySize::Medium: ImGui::Text("Medium"); break;
				case ECompanySize::Large: ImGui::Text("Large"); break;
			}

			ImGui::Separator();
			ImGui::Text("Current State:");
			ImGui::Text("Employees: %d", state.m_Employees);
			ImGui::Text("Profit: $%.2fK", state.m_Profitability);
			ImGui::Text("Liquidity: $%.0fK", state.m_Liquidity);
			ImGui::Text("Revenue: $%.1fK", state.m_LastRevenue);

			ImGui::Separator();
			ImGui::Text("History (last 24 months):");

			// Get history data
			int32_t historyMonths = selectedCompany->GetHistoryMonths();
			int32_t historyIndex = selectedCompany->GetHistoryIndex();

			const float* profitHistory = selectedCompany->GetProfitHistory();
			const float* employeesHistory = selectedCompany->GetEmployeesHistory();
			const float* liquidityHistory = selectedCompany->GetLiquidityHistory();
			const float* revenueHistory = selectedCompany->GetRevenueHistory();

			// Prepare data arrays for ImGui (reorder from oldest to newest)
			std::vector<float> profitValues(historyMonths);
			std::vector<float> employeesValues(historyMonths);
			std::vector<float> liquidityValues(historyMonths);
			std::vector<float> revenueValues(historyMonths);

			for (int32_t i = 0; i < historyMonths; ++i)
			{
				int32_t idx = (historyIndex + i) % historyMonths;
				profitValues[i] = profitHistory[idx];
				employeesValues[i] = employeesHistory[idx];
				liquidityValues[i] = liquidityHistory[idx];
				revenueValues[i] = revenueHistory[idx];
			}

			// Plot Profit History (Green)
			ImGui::Text("Profit (K):");
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
			ImGui::PlotLines("##Profit", profitValues.data(), historyMonths, 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
			ImGui::PopStyleColor();

			// Plot Employees History (Blue)
			ImGui::Text("Employees:");
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.4f, 1.0f, 1.0f));
			ImGui::PlotLines("##Employees", employeesValues.data(), historyMonths, 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
			ImGui::PopStyleColor();

			// Plot Liquidity History (Yellow)
			ImGui::Text("Liquidity (K):");
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
			ImGui::PlotLines("##Liquidity", liquidityValues.data(), historyMonths, 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
			ImGui::PopStyleColor();

			// Plot Revenue History (Cyan)
			ImGui::Text("Revenue (K):");
			ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.0f, 0.8f, 0.8f, 1.0f));
			ImGui::PlotLines("##Revenue", revenueValues.data(), historyMonths, 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));
			ImGui::PopStyleColor();

			ImGui::End();
		}
	}

	// Demo window (can be removed later)
	bool showDemo = true;
	ImGui::ShowDemoWindow(&showDemo);

	// Render ImGui
	ImGui::Render();
}

void CPoliticalGame::Cleanup() {
	std::cout << "Cleaning up Political Game..." << std::endl;

	// Shutdown economy manager
	if (m_EconomyManager)
	{
		m_EconomyManager->Shutdown();
		m_EconomyManager.reset();
	}

	// Shutdown time manager
	if (m_TimeManager)
	{
		m_TimeManager->Shutdown();
		m_TimeManager.reset();
	}

	if (m_World) {
		m_World->Cleanup();
		m_World.reset();
	}

	std::cout << "Political Game cleanup complete!" << std::endl;
}

} // namespace PoliticSim
