#ifndef __Point3__
#define __Point3__

#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include "Vec3.hpp"

struct Point3 {
    float x, y, z;

    // Costruttore
    Point3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    // Conversione in stringa
    std::string to_string() const {
        std::ostringstream oss;
        oss << "Point3(x=" << x << ", y=" << y << ", z=" << z << ")";
        return oss.str();
    }

    // Confronto tra punti (per test)
    bool operator==(const Point3& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    // Somma tra Point3 e Vec3, ritorna un Point3
    Point3 operator+(const Vec3& v) const {
        return Point3(x + v.x, y + v.y, z + v.z);
    }

    // Differenza tra due punti, ritorna un Vec
    Vec3 operator-(const Point3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    // Differenza tra Point3 e Vec, ritorna un Point3
    Point3 operator-(const Vec3& v) const {
        return Point3(x - v.x, y - v.y, z - v.z);
    }

    // Conversione da Point3 a Vec3
    Vec3 to_vec() const {
        return Vec3(x, y, z);
    }
};

#endif 
