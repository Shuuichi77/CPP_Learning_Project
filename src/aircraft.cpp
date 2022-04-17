#include "aircraft.hpp"

#include "GL/opengl_interface.hpp"

#include <cmath>
#include "exception/aircraft_crash.hpp"

void Aircraft::turn_to_waypoint()
{
    assert(!waypoints.empty() && "Aircraft can't turn to empty waypoint");

    Point3D target = waypoints[0];
    if (waypoints.size() > 1)
    {
        const float   d = (waypoints[0] - pos).length();
        const Point3D W = (waypoints[0] - waypoints[1]).normalize(d / 2.0f);
        target += W;
    }

    turn(target - pos - speed);
}

void Aircraft::turn(Point3D direction)
{
    (speed += direction.cap_length(type.max_accel)).cap_length(max_speed());
}

unsigned int Aircraft::get_speed_octant() const
{
    const float speed_len = speed.length();
    if (speed_len > 0)
    {
        const Point3D norm_speed { speed * (1.0f / speed_len) };
        const float   angle = (norm_speed.y() > 0) ? 2.0f * 3.141592f - std::acos(norm_speed.x()) :
                              std::acos(norm_speed.x());
        // partition into NUM_AIRCRAFT_TILES equal pieces
        return (static_cast<int>(std::round((angle * NUM_AIRCRAFT_TILES) / (2.0f * 3.141592f))) + 1) %
               NUM_AIRCRAFT_TILES;
    }
    else
    {
        return 0;
    }
}

// when we arrive at a terminal, signal the tower
void Aircraft::arrive_at_terminal()
{
    assert(is_at_terminal == false && "Aircraft already arrived at terminal");

    // we arrived at a terminal, so start servicing
    control.arrived_at_terminal(*this);
    is_at_terminal = true;
}

// deploy and retract landing gear depending on next waypoints
void Aircraft::operate_landing_gear()
{
    if (waypoints.size() > 1u)
    {
        const auto it            = waypoints.begin();
        const bool ground_before = it->is_on_ground();
        const bool ground_after  = std::next(it)->is_on_ground();
        // deploy/retract landing gear when landing/lifting-off
        if (ground_before && !ground_after)
        {
            std::cout << flight_number << " lift off" << std::endl;
        }
        else if (!ground_before && ground_after)
        {
            std::cout << flight_number << " is now landing..." << std::endl;
            landing_gear_deployed = true;
        }
        else if (!ground_before && !ground_after)
        {
            landing_gear_deployed = false;
        }
    }
}

template<const bool Front>
void Aircraft::add_waypoint(const Waypoint& wp)
{
    if constexpr (Front)
    {
        waypoints.push_front(wp);
    }
    else
    {
        waypoints.push_back(wp);
    }
}

bool Aircraft::move()
{
    if (is_leaving_terminal && waypoints.empty())
    {
        return false;
    }

    // If aircraft is waiting for a terminal, try to reserve a terminal
    if (is_circling())
    {
        WaypointQueue terminal_waypoints = control.reserve_terminal(*this);

        if (!terminal_waypoints.empty())
        {
            waypoints.swap(terminal_waypoints);
        }
    }

    if (waypoints.empty())
    {
        const auto front = false;
        for (const auto& wp: control.get_instructions(*this))
        {
            add_waypoint<front>(wp);
        }
    }

    if (!is_at_terminal)
    {
        turn_to_waypoint();
        // move in the direction of the current speed
        pos += speed;

        // if we are close to our next waypoint, stike if off the list
        if (!waypoints.empty() && distance_to(waypoints.front()) < DISTANCE_THRESHOLD)
        {
            if (waypoints.front().is_at_terminal())
            {
                arrive_at_terminal();
            }
            else
            {
                operate_landing_gear();
            }
            waypoints.pop_front();
        }

        if (is_on_ground())
        {
            if (!landing_gear_deployed)
            {
                throw AircraftCrash { flight_number, pos, speed, "crashed into the ground, bad landing" };
            }
        }
        else
        {
            if (--fuel == 0)
            {
                throw AircraftCrash { flight_number, pos, speed, "out of fuel" };
            }

            // if we are in the air, but too slow, then we will sink!
            const float speed_len = speed.length();
            if (speed_len < SPEED_THRESHOLD)
            {
                pos.z() -= SINK_FACTOR * (SPEED_THRESHOLD - speed_len);
            }
        }

        // update the z-value of the displayable structure
        GL::Displayable::z = pos.x() + pos.y();
    }

    return true;
}

void Aircraft::display() const
{
    type.texture.draw(project_2D(pos), { PLANE_TEXTURE_DIM, PLANE_TEXTURE_DIM }, get_speed_octant());
}

bool Aircraft::has_terminal() const
{
    return control.get_reserved_terminals().find(this) != control.get_reserved_terminals().end();
}

bool Aircraft::is_circling() const
{
    return !has_terminal() && !is_leaving_terminal;
}

bool Aircraft::is_low_on_fuel() const
{
    return fuel < 200u;
}

void Aircraft::refill(unsigned int& fuel_stock)
{
    if (fuel_stock == 0)
    {
        return;
    }

    assert(fuel <= MAX_FUEL && "fuel can't be superior than MAX_FUEL");
    unsigned int fuel_needed = MAX_FUEL - fuel;

    if (fuel_stock > fuel_needed)
    {
        std::cout << flight_number << " has been entirely refilled (+" << fuel_needed << ")" << std::endl;
    }

    else
    {
        std::cout << flight_number << " has not been entirely refilled (+" << fuel_stock << ")" << std::endl;
        fuel_needed = fuel_stock;
    }

    fuel += fuel_needed;
    fuel_stock -= fuel_needed;

    if (fuel_stock == 0)
    {
        std::cout << "There is no fuel_stock available anymore." << std::endl;
    }
}