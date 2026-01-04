# Politic Sim

A political simulation game built with C++20 and the SDL Engine.

## About

Politic Sim is a political strategy simulation game currently in early development. The game uses a custom C++ game engine (SDL Engine) that provides advanced behavior tree systems, component-based architecture, and modern graphics rendering.

## Features

- **Free Camera System**: Navigate the game world with WASD/Arrow keys
- **Component-Based Architecture**: Built on a modular entity-component system
- **Behavior Tree AI**: Powered by SDL Engine's advanced behavior tree system
- **Modern C++**: Built with C++20 standards
- **Cross-Platform**: Supports Linux, Windows, and macOS

## Building

### Prerequisites

- C++20 compatible compiler (GCC 11+, Clang 13+, MSVC 2022+)
- CMake 3.20+
- Git (for cloning with submodules)

### Clone and Build

```bash
# Clone the repository with submodules
git clone --recurse-submodules https://github.com/cucsijuan/politic-sim.git
cd politic-sim

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the game
cmake --build .
```

### Running the Game

```bash
# From the build directory
./PoliticSim
```

## Controls

- **WASD / Arrow Keys**: Move camera
- **P**: Print camera position
- **ESC**: Exit game

## Project Structure

```
politic-sim/
├── include/           # Header files
│   └── politic_game.h
├── src/              # Source files
│   ├── main.cpp
│   ├── politic_game.cpp
│   └── CMakeLists.txt
├── assets/           # Game assets (empty for now)
├── docs/             # Documentation
├── vendor/           # Third-party dependencies
│   └── SDL-Engine/   # Game engine submodule
├── CMakeLists.txt    # Root CMake configuration
└── README.md
```

## Development

The game is built on top of [SDL Engine](https://github.com/cucsijuan/SDL-Engine), a custom C++ game engine featuring:

- Advanced Behavior Tree system with SOA optimization
- Component-based entity system
- Network debugging and profiling tools
- SDL3-based graphics rendering
- ImGui integration for debugging UI

## Current Status

**Version:** 0.1.0 (Pre-alpha)

The game is in early development. Currently implemented:
- Basic game initialization
- Free camera movement
- Input handling system
- World and camera management

## Contributing

This is a personal project currently under active development.

## License

See [LICENSE.txt](LICENSE.txt) for details.

## Acknowledgments

Built with [SDL Engine](https://github.com/cucsijuan/SDL-Engine)
