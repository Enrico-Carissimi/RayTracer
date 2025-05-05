#ifndef __Vec2_
#define __Vec2__

#include <cmath>
#include <sstream>
#include <iostream>
#include "utils.hpp"  // Per areClose()

struct Vec2 {
    float u, v;

    Vec2(float u = 0.0f, float v = 0.0f) : u(u), v(v) {}

    // Conversione a stringa
    std::string toString() const {
        std::ostringstream oss;
        oss << "Vec2 (u = " << u << ", v = " << v << ")";
        return oss.str();
    }

    // Operatori base
    inline Vec2 operator+(const Vec2& other) const { return {u + other.u, v + other.v}; }
    inline Vec2 operator-(const Vec2& other) const { return {u - other.u, v - other.v}; }
    inline Vec2 operator*(float scalar) const { return {u * scalar, v * scalar}; }
    inline Vec2 operator/(float scalar) const { return {u / scalar, v / scalar}; }
    inline Vec2 operator-() const { return {-u, -v}; }

    // Norme
    inline float norm2() const { return u * u + v * v; }
    inline float norm() const { return std::sqrt(norm2()); }

    inline Vec2 normalize() const {
        float n = norm();
        return (n > 0) ? (*this / n) : Vec2();
    }

    // Confronto approssimato
    bool isClose(const Vec2& other, float epsilon = 1e-5f) const {
        return areClose(u, other.u, epsilon) && areClose(v, other.v, epsilon);
    }
};

// Overload operatore di stampa
inline std::ostream& operator<<(std::ostream& stream, const Vec2& v) {
    return stream << v.toString();
}

#endif
