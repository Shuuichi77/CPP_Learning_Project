#include "tower.hpp"

#include "airport.hpp"
#include "terminal.hpp"

#include <cassert>

WaypointQueue Tower::get_circle() const
{
    return {{ Point3D { -1.5f, -1.5f, .5f }, wp_air },
            { Point3D { 1.5f, -1.5f, .5f },  wp_air },
            { Point3D { 1.5f, 1.5f, .5f },   wp_air },
            { Point3D { -1.5f, 1.5f, .5f },  wp_air }};
}

WaypointQueue Tower::get_instructions(Aircraft& aircraft)
{
    if (!aircraft.is_at_terminal)
    {
        // if the aircraft is far, then just guide it to the airport vicinity
        if (aircraft.distance_to(airport.pos) < 5)
        {
            // try and reserve a terminal for the craft to land
            const auto vp = airport.reserve_terminal(aircraft);
            if (!vp.first.empty())
            {
                reserved_terminals.emplace(&aircraft, vp.second);
                return vp.first;
            }
            else
            {
                return get_circle();
            }
        }
        else
        {
            return {};
        }
    }
    else
    {
        // get a path for the craft to start
        const auto it = reserved_terminals.find(&aircraft);
        assert(it != reserved_terminals.end());
        const auto terminal_num = it->second;
        Terminal& terminal = airport.get_terminal(terminal_num);
        if (!terminal.is_servicing())
        {
            terminal.finish_service();
            reserved_terminals.erase(&aircraft);
            aircraft.is_at_terminal      = false;
            aircraft.is_leaving_terminal = true;
            return airport.start_path(terminal_num);
        }
        else
        {
            return {};
        }
    }
}

void Tower::arrived_at_terminal(const Aircraft& aircraft)
{
    assert(aircraft.is_on_ground() && "Aircraft can't arrive at terminal and start service if it isn't on ground");
    const auto it = reserved_terminals.find(&aircraft);
    assert(it != reserved_terminals.end() && "Aircraft can't arrive at terminal if it doesn't have a terminal");
    airport.get_terminal(it->second).start_service(aircraft);
}

WaypointQueue Tower::reserve_terminal(Aircraft& aircraft)
{
    const auto vp = airport.reserve_terminal(aircraft);
    if (!vp.first.empty())
    {
        reserved_terminals.emplace(&aircraft, vp.second);
    }

    return vp.first;
}
void Tower::remove_crashed_aircraft_from_terminal(const Aircraft& aircraft)
{
    const auto it = reserved_terminals.find(&aircraft);
    if (it == reserved_terminals.end())
    {
        return;
    }

    airport.get_terminal(it->second).remove_crashed_aircraft();
    reserved_terminals.erase(&aircraft);;
}