#include "aircraft_manager.hpp"

#include <algorithm>

bool AircraftManager::move()
{
    aircrafts.erase(std::remove_if(aircrafts.begin(), aircrafts.end(),
                                   [](const std::unique_ptr<Aircraft>& aircraft) { return !aircraft->move(); }),
                    aircrafts.end());
    
    return true;
}
void AircraftManager::emplace_aircraft(std::unique_ptr<Aircraft> aircraft)
{
    aircrafts.emplace_back(std::move(aircraft));
}
