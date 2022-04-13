#pragma once

#include "aircraft.hpp"

#include <algorithm>
#include <string>

class AircraftFactory
{
private:
    static constexpr size_t NUM_AIRCRAFT_TYPES = 3;

    AircraftType* _aircraft_types[NUM_AIRCRAFT_TYPES] {};
    std::string                     _airlines[8] = { "AF", "LH", "EY", "DL", "KL", "BA", "AY", "EY" };
    std::unordered_set<std::string> _aircrafts_flight_numbers;

    AircraftType& create_random_aircraft() const { return *(_aircraft_types[rand() % 3]); }

public:
    AircraftFactory()
    {
        _aircraft_types[0] = new AircraftType { .02f, .05f, .02f, MediaPath { "l1011_48px.png" }};
        _aircraft_types[1] = new AircraftType { .02f, .05f, .02f, MediaPath { "b707_jat.png" }};
        _aircraft_types[2] = new AircraftType { .02f * 2, .05f * 2, .02f * 2, MediaPath { "concorde_af.png" }};
    }

    std::unique_ptr<Aircraft> create_aircraft(Tower& tower);
    void print_nb_airline(int airline) const;
};