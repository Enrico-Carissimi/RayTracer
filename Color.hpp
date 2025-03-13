#include <cmath>
#ifndef __Color__
#define __Color__

class Color {
public:
    float r, g, b;

    Color() : r(0.0f), g(0.0f), b(0.0f) {}
    Color(float r, float g, float b) : r(r), g(g), b(b) {}

    Color operator*(const Color& other) const {
        return Color(r * other.r, g * other.g, b * other.b);
    }

    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b);
    }

    Color operator*(float scalar) const {
        return Color(r * scalar, g * scalar, b * scalar);
    }

    bool isClose(const Color& other, float epsilon = 1e-5f) const {
        return (std::fabs(r - other.r) < epsilon) &&
               (std::fabs(g - other.g) < epsilon) &&
               (std::fabs(b - other.b) < epsilon);
    }
    
};

#endif