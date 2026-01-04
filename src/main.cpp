#include <Engine/Engine.h>
#include "politic_game.h"
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
	Engine::CEngine engine;

	if (!engine.Initialize("Politic Sim", 1024, 768)) {
		std::cerr << "Failed to initialize engine!" << std::endl;
		return -1;
	}

	// Create and configure the game application
	auto game = std::make_unique<PoliticSim::CPoliticalGame>();
	engine.SetApplication(std::move(game));

	// Run the game loop
	engine.Run();

	return 0;
}
