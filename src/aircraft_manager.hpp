#pragma once

#include "aircraft.hpp"

#include <memory>
#include <vector>

class AircraftManager : public GL::DynamicObject
{
private:
    std::vector<std::unique_ptr<Aircraft>> aircrafts;

public:
    virtual ~AircraftManager() = default;
    bool move() override;
    void emplace_aircraft(std::unique_ptr<Aircraft> aircraft);
    int get_required_fuel() const;
};