#include "aircraft_manager.hpp"

#include <algorithm>

bool AircraftManager::move()
{
    std::sort(aircrafts.begin(), aircrafts.end(),
              [](const std::unique_ptr<Aircraft>& a, const std::unique_ptr<Aircraft>& b) {
                  if (a->has_terminal() && !b->has_terminal())
                  {
                      return true;
                  }

                  if (!a->has_terminal() && b->has_terminal())
                  {
                      return false;
                  }

                  return a->get_fuel() < b->get_fuel();;
              });

    aircrafts.erase(std::remove_if(aircrafts.begin(), aircrafts.end(),
                                   [this](const std::unique_ptr<Aircraft>& aircraft) {
                                       try
                                       {
                                           return !aircraft->move();
                                       }
                                       catch (const std::runtime_error& err)
                                       {
                                           std::cerr << "AircraftCrash: " << err.what() << std::endl;
                                           nb_crash++;
                                           return true;
                                       }
                                   }), aircrafts.end());
    return true;
}
void AircraftManager::emplace_aircraft(std::unique_ptr<Aircraft> aircraft)
{
    aircrafts.emplace_back(std::move(aircraft));
}

unsigned int AircraftManager::get_required_fuel() const
{
    const auto get_aircraft_required_fuel = [](const std::unique_ptr<Aircraft>& aircraft) {
        return (aircraft->is_low_on_fuel() && aircraft->is_on_ground()) ? (3000 - aircraft->get_fuel()) : 0;
    };

    return std::accumulate(aircrafts.begin(), aircrafts.end(), 0,
                           [&get_aircraft_required_fuel](int current, const std::unique_ptr<Aircraft>& aircraft) {
                               return current + get_aircraft_required_fuel(aircraft);
                           });
}