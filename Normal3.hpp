#ifndef __Normal3__
#define __Normal3__

#include <iostream>
#include <cmath>
#include <string>
#include "Vec3.hpp"

struct Normal3 {
    float x, y, z;

    // Costruttore
    Normal3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    // Conversione a stringa
    std::string to_string() const {
        return "Normal3(x=" + std::to_string(x) + ", y=" + std::to_string(y) + ", z=" + std::to_string(z) + ")";
    }

    // Confronto tra normali
    bool operator==(const Normal3& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    // Negazione
    Normal3 operator-() const { return Normal3(-x, -y, -z); }

    // Moltiplicazione per scalare
    Normal3 operator*(float scalar) const { return Normal3(x * scalar, y * scalar, z * scalar); }

    // Prodotto scalare Vec · Normal3
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }

    // Prodotto scalare Normal3 · Normal3
    float dot(const Normal3& n) const { return x * n.x + y * n.y + z * n.z; }

    // Prodotto vettoriale Vec × Normal3
    Vec3 cross(const Vec3& v) const {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    // Prodotto vettoriale Normal3 × Normal3
    Normal3 cross(const Normal3& n) const {
        return Normal3(
            y * n.z - z * n.y,
            z * n.x - x * n.z,
            x * n.y - y * n.x
        );
    }

    // Norma al quadrato ∥n∥²
    float squared_norm() const { return x * x + y * y + z * z; }

    // Norma ∥n∥
    float norm() const { return std::sqrt(squared_norm()); }

    // Normalizzazione n → n / ∥n∥
    Normal3 normalize() const {
        float n = norm();
        return (n > 0) ? Normal3(x / n, y / n, z / n) : Normal3();
    }

    // Conversione da Vec a Normal3
    explicit Normal3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
};

#endif 
