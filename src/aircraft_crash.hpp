#pragma once

#include <stdexcept>
#include "geometry.hpp"

class AircraftCrash : public std::runtime_error
{
private:
    static std::string build_error_msg(const std::string& flight_number, const Point3D& pos, const Point3D& speed,
                                       const std::string& error)
    {
        using namespace std::string_literals;

        return flight_number + " crashed with pos = (" +
               std::to_string(pos.x()) + ", " + std::to_string(pos.y()) + ", " + std::to_string(pos.z()) +
               "), speed = " + std::to_string(speed.length()) +
               ": " + error;
    }

public:
    AircraftCrash(const std::string& flight_number, const Point3D& pos, const Point3D& speed, const std::string& error)
            : std::runtime_error { build_error_msg(flight_number, pos, speed, error) } {}
};