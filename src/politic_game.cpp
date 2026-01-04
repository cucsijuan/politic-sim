#include "politic_game.h"

#include <Engine/Core/Camera/CameraManager.h>
#include <iostream>
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

	// Demo window (can be removed later)
	bool showDemo = true;
	ImGui::ShowDemoWindow(&showDemo);

	// Render ImGui
	ImGui::Render();
}

void CPoliticalGame::Cleanup() {
	std::cout << "Cleaning up Political Game..." << std::endl;

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
