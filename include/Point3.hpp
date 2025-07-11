#ifndef __Point3__
#define __Point3__

#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include "Vec3.hpp"
#include "utils.hpp"

/**
 * @brief Represents a 3D point in space with coordinates (x, y, z).
 * 
 * Supports basic vector arithmetic with Vec3 and comparison for approximate equality.
 */
struct Point3 {
    float x, y, z;

    Point3(float x = 0., float y = 0., float z = 0.) : x(x), y(y), z(z) {}

    // Conversion to string
    std::string toString() const {
        std::ostringstream oss;
        oss << "Point3 (x = " << x << ", y = " << y << ", z = " << z << ")";
        return oss.str();
    }

    // Sum and difference between Point3 e Vec3, returns a Point3
    inline Point3 operator+(const Vec3& v) const {
        return Point3(x + v.x, y + v.y, z + v.z);
    }
    inline Point3 operator-(const Vec3& v) const {
        return Point3(x - v.x, y - v.y, z - v.z);
    }

    // Difference between two points, returns a Vec3
    inline Vec3 operator-(const Point3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    // Scalar multiplication
    inline Point3 operator*(float scalar) const {
    return Point3(x * scalar, y * scalar, z * scalar);
    }

    // Conversion to Vec3
    inline Vec3 toVec() const { return Vec3(x, y, z); }

    // Comparison for tests
    bool isClose(Point3 other, float epsilon = 1e-5f) {
        return areClose(*this, other, epsilon);
    }
};

inline std::ostream& operator<<(std::ostream& stream, const Point3& p) { return stream << p.toString(); }

#endif