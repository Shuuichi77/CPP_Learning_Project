#include "aircraftManager.hpp"

bool AircraftManager::move()
{
    for (auto it = aircrafts.begin(); it != aircrafts.end();)
    {
        if (!(*it)->move())
        {
            aircrafts.erase(it);
        }

        else
        {
            ++it;
        }
    }

    return true;
}
void AircraftManager::emplace_aircraft(std::unique_ptr<Aircraft> aircraft)
{
    aircrafts.emplace_back(std::move(aircraft));
}
