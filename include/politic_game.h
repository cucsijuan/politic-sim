#pragma once

#include <Engine/Engine.h>
#include <Engine/Core/World/World.h>
#include <Engine/Core/Camera/CameraFollowGO.h>
#include "Time/CTimeManager.h"
#include "Economy/CEconomyManager.h"
#include <memory>

namespace PoliticSim {

/**
 * Main game application class for the Political Simulation game
 * Implements the IApplication interface from the SDL Engine
 */
class CPoliticalGame : public Engine::IApplication {
private:
	std::unique_ptr<CWorld> m_World;
	std::unique_ptr<CCameraFollowGO> m_Camera;
	std::unique_ptr<CTimeManager> m_TimeManager;
	std::unique_ptr<CEconomyManager> m_EconomyManager;
	const bool* m_KeyboardState;

	int32_t m_SelectedCompanyID;

	static constexpr float CAMERA_SPEED = 200.0f;

	// Helper methods
	void HandleDiscreteInput(const SDL_Event& event);
	void HandleContinuousInput(float deltaTime);
	void UpdateCameraMovement(float deltaTime);

public:
	CPoliticalGame() : m_SelectedCompanyID(-1) {}
	virtual ~CPoliticalGame() = default;

	// IApplication implementation
	bool Initialize() override;
	void HandleInput(const SDL_Event& event) override;
	void Update(float deltaTime) override;
	void Render(CRenderer& renderer) override;
	void Cleanup() override;

	// Accessors
	CCameraFollowGO* GetCamera() const { return m_Camera.get(); }
	CWorld* GetWorld() const { return m_World.get(); }
	CTimeManager* GetTimeManager() const { return m_TimeManager.get(); }
	CEconomyManager* GetEconomyManager() const { return m_EconomyManager.get(); }
};

} // namespace PoliticSim
