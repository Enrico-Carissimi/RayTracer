#ifndef __Vec3__
#define __Vec3__

#include <iostream>
#include <cmath>
#include <sstream>
#include "utils.hpp"

/**
 * @brief Represents a 3D vector with coordinates (x, y, z).
 */
struct Vec3 {
    float x, y, z;

    Vec3(float x = 0., float y = 0., float z = 0.) : x(x), y(y), z(z) {}

    // Conversion to string
    std::string toString() const {
        std::ostringstream oss;
        oss << "Vec3 (x = " << x << ", y = " << y << ", z = " << z << ")";
        return oss.str();
    }

    // Sum and difference
    inline Vec3 operator+(const Vec3& other) const { return {x + other.x, y + other.y, z + other.z}; }
    inline Vec3 operator-(const Vec3& other) const { return {x - other.x, y - other.y, z - other.z}; }

    // Scalar multiplication and negation
    inline Vec3 operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    inline Vec3 operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }
    inline Vec3 operator-() const { return {-x, -y, -z}; }
    
    // Squared norm and norm
    inline float norm2() const { return x * x + y * y + z * z; }
    inline float norm() const { return std::sqrt(norm2()); }
    
    // Normalize the Vector
    inline Vec3 normalize() const {
        float n = norm();
        return (n > 0) ? (*this / n) : Vec3();
    }

    // Comparison for tests
    bool isClose(Vec3 other, float epsilon = 1e-5f) {
        return areClose(*this, other, epsilon);
    }
};

// Dot product
inline float dot(const Vec3& v, const Vec3& u) { return v.x * u.x + v.y * u.y + v.z * u.z; }

// Cross product
inline Vec3 cross(const Vec3& v, const Vec3& u) {
    return {
        v.y * u.z - v.z * u.y,
        v.z * u.x - v.x * u.z,
        v.x * u.y - v.y * u.x
    };
}

inline std::ostream& operator<<(std::ostream& stream, const Vec3& v) { return stream << v.toString(); }

#endif