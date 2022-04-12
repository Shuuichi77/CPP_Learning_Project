#include "aircraft_factory.hpp"

std::unique_ptr<Aircraft> AircraftFactory::create_random_aircraft(Tower& tower)
{
    const AircraftType& type = *(_aircraft_types[rand() % 3]);
    const std::string flight_number = _airlines[std::rand() % 8] + std::to_string(1000 + (rand() % 9000));
    const float angle = (rand() % 1000) * 2 * 3.141592f / 1000.f; // random angle between 0 and 2pi
    const Point3D start = Point3D { std::sin(angle), std::cos(angle), 0 } * 3 + Point3D { 0, 0, 2 };
    const Point3D direction = (-start).normalize();

    return std::make_unique<Aircraft>(type, flight_number, start, direction, tower);
}