# Politic Sim executable
add_executable(PoliticSim)

target_sources(PoliticSim
  PRIVATE
    main.cpp
    politic_game.cpp
    Time/CTimeUnits.cpp
    Time/CGameClock.cpp
    Time/CTimeScale.cpp
    Time/CTimeManager.cpp
)

# Include directories
target_include_directories(PoliticSim
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor/SDL-Engine/Engine/include
)

# Link with engine
target_link_libraries(PoliticSim
  PRIVATE
    SDLEngine
)

# Set C++ standard
set_target_properties(PoliticSim PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)
