#pragma once

#include "aircraft_factory.hpp"
#include "aircraft_manager.hpp"

class Airport;

struct AircraftType;

struct ContextInitializer
{
public:
    ContextInitializer(int argc, char** argv)
    {
        MediaPath::initialize(argv[0]);
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        GL::init_gl(argc, argv, "Airport Tower Simulation");
    }
};

class TowerSimulation
{
private:
    bool                     help = false;
    std::unique_ptr<Airport> airport;
    ContextInitializer       _context_initializer;
    AircraftManager          aircraft_manager;
    AircraftFactory          aircraft_factory;

    TowerSimulation(const TowerSimulation&) = delete;
    TowerSimulation& operator=(const TowerSimulation&) = delete;

    void create_aircraft();
    void create_keystrokes();
    void display_help() const;
    void init_airport();
    void init_aircraftManager();

public:
    TowerSimulation(int argc, char** argv);
    ~TowerSimulation();
    void launch();
};
