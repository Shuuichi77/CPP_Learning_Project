#pragma once

#include "GL/displayable.hpp"
#include "aircraft_types.hpp"
#include "config.hpp"
#include "geometry.hpp"
#include "tower.hpp"
#include "waypoint.hpp"

#include <string>
#include <string_view>

class Aircraft : public GL::Displayable
{
private:
    static constexpr unsigned int MAX_FUEL = 3000;
    static constexpr unsigned int MIN_FUEL = 150;

    const AircraftType& type;
    const std::string flight_number;
    Point3D           pos, speed; // note: the speed should always be normalized to length 'speed'
    WaypointQueue     waypoints            = {};
    Tower& control;
    bool         landing_gear_deployed = false; // is the landing gear deployed?
    bool         is_at_terminal        = false;
    bool         is_leaving_terminal   = false;
    unsigned int fuel                  = MIN_FUEL + (rand() % (MAX_FUEL - MIN_FUEL)) + 1;

    // turn the aircraft to arrive at the next waypoint
    // try to facilitate reaching the waypoint after the next by facing the
    // right way to this end, we try to face the point Z on the line spanned by
    // the next two waypoints such that Z's distance to the next waypoint is
    // half our distance so: |w1 - pos| = d and [w1 - w2].normalize() = W and Z
    // = w1 + W*d/2
    void turn_to_waypoint();
    void turn(Point3D direction);

    // select the correct tile in the plane texture (series of 8 sprites facing
    // [North, NW, W, SW, S, SE, E, NE])
    unsigned int get_speed_octant() const;
    // when we arrive at a terminal, signal the tower
    void arrive_at_terminal();
    // deploy and retract landing gear depending on next waypoints
    void operate_landing_gear();
    void add_waypoint(const Waypoint& wp, const bool front);
    float max_speed() const { return is_on_ground() ? type.max_ground_speed : type.max_air_speed; }

    Aircraft(const Aircraft&) = delete;
    Aircraft& operator=(const Aircraft&) = delete;

public:
    Aircraft(const AircraftType& type_, const std::string_view& flight_number_, const Point3D& pos_,
             const Point3D& speed_, Tower& control_)
            :
            GL::Displayable { pos_.x() + pos_.y() },
            type { type_ },
            flight_number { flight_number_ },
            pos { pos_ },
            speed { speed_ },
            control { control_ }
    {
        assert(!flight_number.empty());
        speed.cap_length(max_speed());
    }

    virtual ~Aircraft() { control.remove_crashed_aircraft_from_terminal(*this); }

    const std::string& get_flight_num() const { return flight_number; }
    unsigned int get_fuel() const { return fuel; }
    unsigned int get_max_fuel() const { return MAX_FUEL; }

    float distance_to(const Point3D& p) const { return pos.distance_to(p); }
    void display() const override;
    bool move();
    bool has_terminal() const;
    bool is_circling() const;
    bool is_low_on_fuel() const;
    bool is_on_ground() const { return pos.z() < DISTANCE_THRESHOLD; }
    void refill(unsigned int& fuel_stock);

    friend class Tower;
};
