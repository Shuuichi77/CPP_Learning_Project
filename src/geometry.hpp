#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>

template<typename T, const size_t Size>
class Point
{
private:
    std::array<T, Size> values;

public:
    Point() = default;

    template<typename... Args>
    Point(T head, Args&& ... args)
            : values { head, std::forward<Args>(args)... }
    {
        static_assert(sizeof...(args) + 1 == Size && "Parameters numbers must be equal to Size");
    }

    std::array<T, Size> get_values() const { return values; }

    T& operator[](size_t index)
    {
        assert(index < Size);
        return values[index];
    }

    const T& operator[](size_t index) const
    {
        assert(index < Size);
        return values[index];
    }

    float& x()
    {
        return values[0];
    }
    float x() const
    {
        return values[0];
    }

    float& y()
    {
        static_assert(Size > 1);
        return values[1];
    }
    float y() const
    {
        static_assert(Size > 1);
        return values[1];
    }

    float& z()
    {
        static_assert(Size > 2);
        return values[2];
    }
    float z() const
    {
        static_assert(Size > 2);
        return values[2];
    }

    Point& operator+=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(),
                       std::plus<T> {});
        return *this;
    }

    Point& operator-=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(),
                       std::minus<T> {});
        return *this;
    }

    Point& operator*=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(),
                       std::multiplies<T> {});
        return *this;
    }

    Point& operator*=(const T scalar)
    {
        std::transform(values.begin(), values.end(), values.begin(),
                       [scalar](T value) -> T { return value * scalar; });
        return *this;
    }

    Point operator+(const Point& other) const
    {
        Point result = *this;
        result += other;
        return result;
    }

    Point operator-(const Point& other) const
    {
        Point result = *this;
        result -= other;
        return result;
    }

    Point operator*(const Point& other) const
    {
        Point result = *this;
        result *= other;
        return result;
    }

    Point operator*(const T scalar) const
    {
        Point result = *this;
        result *= scalar;
        return result;
    }

    Point operator-() const
    {
        Point<T, Size> new_point = {};
        std::transform(values.begin(), values.end(), new_point.values.begin(),
                       [](T e) { return -e; });

        return new_point;
    }

    T length() const
    {
        return std::sqrt(std::accumulate(values.begin(), values.end(), (T) 0,
                                         [](T current, T next) {
                                             return current + (next * next);
                                         }));
    }

    T distance_to(const Point& other) const { return (*this - other).length(); }

    Point& normalize(const T target_len = (T) 1)
    {
        const T current_len = length();
        if (current_len == 0)
        {
            throw std::logic_error("cannot normalize vector of length 0");
        }

        *this *= (target_len / current_len);
        return *this;
    }

    Point& cap_length(const T max_len)
    {
        assert(max_len > 0);

        const T current_len = length();
        if (current_len > max_len)
        {
            *this *= (max_len / current_len);
        }

        return *this;
    }
};

using Point2D = Point<float, 2>;
using Point3D = Point<float, 3>;

// our 3D-coordinate system will be tied to the airport: the runway is parallel to the x-axis, the z-axis
// points towards the sky, and y is perpendicular to both thus,
// {1,0,0} --> {.5,.5}   {0,1,0} --> {-.5,.5}   {0,0,1} --> {0,1}
inline Point2D project_2D(const Point3D& p)
{
    return { .5f * p.x() - .5f * p.y(), .5f * p.x() + .5f * p.y() + p.z() };
}

inline void test_generic_points()
{
    Point<int, 3> p1 {};
    Point<int, 3> p2 {};

    p1[0] = 1;
    p1[1] = 2;
    p1[2] = 4;

    p2[0] = -1;
    p2[1] = -1;
    p2[2] = -1;
    // p1 = (1, 2, 4)
    // p2 = (-1, -1, -1)

    p1 += p2;
    // Expected : p1 = (0, 1, 3)
    std::cout << "p1 += : " << p1[0] << ", " << p1[1] << ", " << p1[2] << std::endl;

    p1 -= p2;
    // Expected : p1 = (1, 2, 4)
    std::cout << "p1 -= : " << p1[0] << ", " << p1[1] << ", " << p1[2] << std::endl;

    p1 *= p2;
    // Expected : p1 = (-1, -2, -4)
    std::cout << "p1 *= : " << p1[0] << ", " << p1[1] << ", " << p1[2] << std::endl;

    auto p3 = p1 + p2;
    // Expected : p3 = (-2, -3, -5)
    std::cout << "p3 = p1 + p2: " << p3[0] << ", " << p3[1] << ", " << p3[2] << std::endl;

    p3 = p1 - p2;
    // Expected : p3 = (0, -1, -3)
    std::cout << "p3 = p1 - p2 : " << p3[0] << ", " << p3[1] << ", " << p3[2] << std::endl;

    p3 = p1 * p2;
    // Expected : p3 = (1, 2, 4)
    std::cout << "p3 = p1 * p2 : " << p3[0] << ", " << p3[1] << ", " << p3[2] << std::endl;

    p3 = -p1;
    // Expected : p3 = (1, 2, 4)
    std::cout << "p3 = -p1 : " << p3[0] << ", " << p3[1] << ", " << p3[2] << std::endl;
}