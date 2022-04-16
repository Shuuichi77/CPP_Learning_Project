#include "tower_sim.hpp"

#include "GL/opengl_interface.hpp"
#include "airport.hpp"
#include "config.hpp"
#include "img/image.hpp"
#include "img/media_path.hpp"

#include <cassert>

using namespace std::string_literals;

TowerSimulation::TowerSimulation(int argc, char** argv)
        : help { argc > 1 &&
                 (std::string { argv[1] } == "--help"s ||
                  std::string { argv[1] } == "-h"s) },
          _context_initializer { ContextInitializer(argc, argv) }
{
    create_keystrokes();
}

TowerSimulation::~TowerSimulation()
{
    delete airport;
}

void TowerSimulation::create_aircraft()
{
    assert(airport); // make sure the airport is initialized before creating aircraft
    aircraft_manager.emplace_aircraft(aircraft_factory.create_aircraft(airport->get_tower()));
}

void TowerSimulation::create_keystrokes()
{
    GL::keystrokes.emplace('x', []() { GL::exit_loop(); });
    GL::keystrokes.emplace('q', []() { GL::exit_loop(); });
    GL::keystrokes.emplace('c', [this]() { create_aircraft(); });
    GL::keystrokes.emplace('+', []() { GL::change_zoom(0.95f); });
    GL::keystrokes.emplace('-', []() { GL::change_zoom(1.05f); });
    GL::keystrokes.emplace('f', []() { GL::toggle_fullscreen(); });
    GL::keystrokes.emplace('a', []() { GL::decrease_framerate(); });
    GL::keystrokes.emplace('z', []() { GL::increase_framerate(); });
    GL::keystrokes.emplace('p', []() { GL::pause(); });
    GL::keystrokes.emplace('m', [this]() { aircraft_manager.print_nb_crash(); });

    for (int i = 0 ; i < 8 ; ++i)
    {
        GL::keystrokes.emplace(char(i + 48), [this, i]() { aircraft_factory.print_nb_airline(i); });
    }
}

void TowerSimulation::display_help() const
{
    std::cout << "This is an airport tower simulator" << std::endl
              << "the following keysstrokes have meaning:" << std::endl;
    for (const auto&[key, _]: GL::keystrokes)
    {
        std::cout << key << ' ';
    }

    std::cout << std::endl;
}

void TowerSimulation::init_airport()
{
    airport = new Airport { aircraft_manager, one_lane_airport, Point3D { 0, 0, 0 },
                            new img::Image { one_lane_airport_sprite_path.get_full_path() }};

    GL::display_queue.emplace_back(airport);
    GL::move_queue.emplace(airport);
}

void TowerSimulation::init_aircraftManager()
{
    GL::move_queue.emplace(&aircraft_manager);
}

void TowerSimulation::launch()
{
    if (help)
    {
        display_help();
        return;
    }

    init_aircraftManager();
    init_airport();

    GL::loop();
}
