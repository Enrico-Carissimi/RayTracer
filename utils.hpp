#ifndef __utils__
#define __utils__

#include <iostream>
#include <source_location>
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

/**
 * @brief Simple assert function to avoid using the standard one.
 * 
 * This has to be done to avoid problems using the Visual Studio compiler in debug mode,
 * and to test in release mode, since normal asserts are discarded.
 * The starting "s" stands for "simple".
 * 
 * @param expr the expression to evaluate
 * @param loc automatic, the place in the source code where sassert is called
 */
void sassert(bool expr, const std::source_location loc = std::source_location::current()) {
    if (!expr) {
        std::cout << "ERROR: assertion failed in function \"" << loc.function_name() << "\", in file \"" << loc.file_name() << "\" on line " << loc.line() << std::endl;
        exit(-1);
    }
}

#endif