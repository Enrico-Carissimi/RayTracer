#ifndef __Vec3__
#define __Vec3__

#include <iostream>
#include <cmath>
#include <sstream>

struct Vec3 {
    float x, y, z;


    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    // Conversion to string
    std::string toString() const {
        std::ostringstream oss;
        oss << "Vec3(x=" << x << ", y=" << y << ", z=" << z << ")";
        return oss.str();
    }

    // Sum and difference
    Vec3 operator+(const Vec3& other) const { return {x + other.x, y + other.y, z + other.z}; }
    Vec3 operator-(const Vec3& other) const { return {x - other.x, y - other.y, z - other.z}; }

    // Scalar multiplication and negation
    Vec3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    Vec3 operator-() const { return {-x, -y, -z}; }

    // Dot product
    float dot(const Vec3& other) const { return x * other.x + y * other.y + z * other.z; }

    // Cross product
    Vec3 cross(const Vec3& other) const {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    // Squared norm and norm
    float squaredNorm() const { return x * x + y * y + z * z; }
    float norm() const { return std::sqrt(squaredNorm()); }

    // Normalize the Vec3tor
    Vec3 normalized() const {
        float n = norm();
        return (n > 0) ? (*this * (1.0f / n)) : Vec3{0, 0, 0};
    }
};

#endif 