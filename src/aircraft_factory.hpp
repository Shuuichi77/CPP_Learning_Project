#pragma once

#include "aircraft.hpp"

#include <algorithm>
#include <string>

class AircraftFactory
{
private:
    static constexpr size_t NB_AIRCRAFT_TYPES = 3;
    static constexpr size_t NB_AIRLINES       = 8;

    const std::array<AircraftType, NB_AIRCRAFT_TYPES> _aircraft_types;
    const std::array<std::string, NB_AIRLINES>        _airlines;
    std::unordered_set<std::string>                   _aircrafts_flight_numbers;

public:
    AircraftFactory()
            : _aircraft_types { AircraftType { .02f, .05f, .02f, MediaPath { "l1011_48px.png" }},
                                AircraftType { .02f, .05f, .02f, MediaPath { "b707_jat.png" }},
                                AircraftType { .02f * 2, .05f * 2, .02f * 2, MediaPath { "concorde_af.png" }}},
              _airlines { "AF", "LH", "EY", "DL", "KL", "BA", "AY", "EY" } {}

    std::unique_ptr<Aircraft> create_aircraft(Tower& tower);
    void print_nb_airline(size_t airline) const;
};