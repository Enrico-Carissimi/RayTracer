#ifndef __Color__
#define __Color__

#include <cmath>
#include <algorithm>

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

    Color& operator+=(const Color& other) {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }

    Color& operator*=(float scalar) {
        r *= scalar;
        g *= scalar;
        b *= scalar;
        return *this;
    }

    bool isClose(const Color& other, float epsilon = 1e-5f) const {
        return (std::fabs(r - other.r) < epsilon) &&
               (std::fabs(g - other.g) < epsilon) &&
               (std::fabs(b - other.b) < epsilon);
    }

    float luminosity() const {
        float maxVal = std::max({r, g, b});
        float minVal = std::min({r, g, b});

        return (maxVal + minVal) * 0.5f; 
    }
};

std::ostream& operator<<(std::ostream& stream, const Color& color) {
    return stream << "Color (r = " << color.r << ", g = " << color.g << ", b = " << color.b << ")";
}

#endif