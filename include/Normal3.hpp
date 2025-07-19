#ifndef __Normal3__
#define __Normal3__

#include <iostream>
#include <cmath>
#include <string>
#include "Vec3.hpp"

/**
 * @struct Normal3
 * @brief Represents a 3D normal vector with basic operations.
 */
struct Normal3 {
    float x, y, z;

    Normal3(float x = 0., float y = 0., float z = 0.) : x(x), y(y), z(z) {}
    explicit Normal3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}

    // Conversion to string
    std::string toString() const {
        std::ostringstream oss;
        oss << "Normal3 (x = " << x << ", y = " << y << ", z = " << z << ")";
        return oss.str();
    }

    // Negation
    inline Normal3 operator-() const { return Normal3(-x, -y, -z); }

    // Multiplication and division by scalar
    inline Normal3 operator*(float scalar) const { return Normal3(x * scalar, y * scalar, z * scalar); }
    inline Normal3 operator/(float scalar) const { return Normal3(x / scalar, y / scalar, z / scalar); }

    // Norm
    inline float norm2() const { return x * x + y * y + z * z; }
    inline float norm() const { return std::sqrt(norm2()); }

    // Normalization
    inline Normal3 normalize() const {
        float n = norm();
        return (n > 0) ? Normal3(x / n, y / n, z / n) : Normal3();
    }

    Vec3 toVec() const { return Vec3(x, y, z); }

    // Comparison for tests
    bool isClose(Normal3 other, float epsilon = 1e-5f) {
        return areClose(*this, other, epsilon);
    }
};

// Dot and cross products between Normals and with Vec
inline float dot(const Normal3& n, const Vec3& v) { return n.x * v.x + n.y * v.y + n.z * v.z; }
inline float dot(const Vec3& v, const Normal3& n) { return dot(n, v); } // explicit's fault
inline float dot(const Normal3& n, const Normal3& m) { return n.x * m.x + n.y * m.x + n.z * m.z; }

inline Vec3 cross(const Normal3& n, const Vec3& v) {
    return Vec3(
        n.y * v.z - n.z * v.y,
        n.z * v.x - n.x * v.z,
        n.x * v.y - n.y * v.x
    );
}
inline Vec3 cross(const Vec3& v, const Normal3& n) { return cross(n, v); } // explicit's fault
inline Normal3 cross(const Normal3& n, const Normal3& m) {
    return Normal3(
        n.y * m.z - n.z * m.y,
        n.z * m.x - n.x * m.z,
        n.x * m.y - n.y * m.x
    );
}

inline std::ostream& operator<<(std::ostream& stream, const Normal3& n) { return stream << n.toString(); }

#endif