#ifndef __utils__
#define __utils__

#include <cmath>
#include <string>

#define PI 3.1415926535897932385

inline float degToRad(float degrees){return degrees * PI / 180.;}

bool areClose(float a, float b, float epsilon = 1e-5f) {
    return std::fabs(a - b) < epsilon;
}

bool areCloseMatrix(const float A[16], const float B[16], float epsilon = 1e-4f) {
    for (int i = 0; i < 16; i++) {
        if (!areClose(A[i], B[i], epsilon)) return false;
    }
    return true;
}

template <typename T1>
bool areClose(const T1& v, const T1& u, float epsilon = 1e-5f) {
    return (areClose(v.x, u.x, epsilon) && areClose(v.y, u.y, epsilon) && areClose(v.z, u.z, epsilon));
}

#endif