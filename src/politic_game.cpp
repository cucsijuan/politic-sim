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

	std::cout << "Political Game initialized successfully!" << std::endl;
	std::cout << "Controls: WASD to move camera" << std::endl;
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
	HandleContinuousInput(deltaTime);

	if (m_World) {
		m_World->Update(deltaTime);
	}

	if (m_Camera) {
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
	ImGui::Text("ESC: Exit");
	ImGui::End();

	// Demo window (can be removed later)
	bool showDemo = true;
	ImGui::ShowDemoWindow(&showDemo);

	// Render ImGui
	ImGui::Render();
}

void CPoliticalGame::Cleanup() {
	std::cout << "Cleaning up Political Game..." << std::endl;

	if (m_World) {
		m_World->Cleanup();
		m_World.reset();
	}

	std::cout << "Political Game cleanup complete!" << std::endl;
}

} // namespace PoliticSim
