#ifndef __utils__
#define __utils__

#include <iostream>
#include <source_location>
#include <cmath>
#include <string>
#include <cstdint>
#include <limits>
#include <numbers>
#include <unordered_map>

inline constexpr float PI = std::numbers::pi_v<float>; // c++20
inline constexpr float INV_PI = 1.0f / PI;
inline constexpr float INF = std::numeric_limits<float>::infinity();
inline constexpr float RAY_MIN = 1e-5f;

struct Vec3; // avoid circular inclusion
struct Normal3;



inline float degToRad(float degrees){ return degrees * PI / 180.0f; }

inline float clamp(float x) { return x / (1 + x); }

inline bool areClose(float a, float b, float epsilon = 1e-5f) {
    return std::fabs(a - b) < epsilon;
}

inline bool areCloseMatrix(const float A[16], const float B[16], float epsilon = 1e-4f) {
    for (int i = 0; i < 16; i++) {
        if (!areClose(A[i], B[i], epsilon)) return false;
    }
    return true;
}

template <typename T1>
inline bool areClose(const T1& v, const T1& u, float epsilon = 1e-5f) {
    return (areClose(v.x, u.x, epsilon) && areClose(v.y, u.y, epsilon) && areClose(v.z, u.z, epsilon));
}

// geometric transformations on vectors, v and n must be normalized
Vec3 reflect(const Vec3& v, const Normal3& n);
Vec3 refract(const Vec3& v, const Normal3& n, float refractionIndexRatio);

// utility for the lexer
inline bool isCharSkippable(const char& c) {
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '#');
}

// utility for the render command
/**
 * @brief Validate a string in the format "name:value", where value is convertible to float.
 * 
 * Used to define a float variable used in a scene file from command line.
 * 
 * @param s String to validate.
 * @param floatVariables Container for name-value pairs.
 */
void validateFloatVariable(std::string& s, std::unordered_map<std::string, float>& floatVariables);



/**
 * @brief Simple assert function to avoid using the standard one.
 * 
 * This has to be done to avoid problems using the Visual Studio compiler in debug mode,
 * and to test in release mode, since normal asserts are discarded.
 * The starting "s" stands for "simple".
 * 
 * @param expr The expression to evaluate.
 * @param loc Automatic, the place in the source code where sassert is called.
 */
inline void sassert(bool expr, const std::source_location loc = std::source_location::current()) {
    if (!expr) {
        std::cout << "ERROR: assertion failed in function \"" << loc.function_name() << "\", in file \"" << loc.file_name() << "\" on line " << loc.line() << std::endl;
        exit(-1);
    }
}

/**
 * @brief Tests if an exception is thrown by a function with the specified parameter.
 * 
 * If the function accepts more parameters, or if it is a constructor, use a lambda:
 * testException(parameter, [](Parameter p) -> {return function(p, [...]);});.
 * 
 * @tparam Parameter 
 * @tparam Function 
 * @param parameter Parameter to pass to the function.
 * @param function The function throwing the exeption to test.
 */
template<typename Parameter, typename Function>
void testException(Parameter& parameter, Function function) {
    bool exceptionThrown = false;
    try{function(parameter);}
    catch(std::exception& e){exceptionThrown = true;}
    sassert(exceptionThrown);
}



/**
 * @brief Permuted congruential generator, RNG used instead of the default one.
 * 
 * See https://www.pcg-random.org/.
 * 
 */
class PCG {
public:
    uint64_t state;
    uint64_t inc;

    PCG(uint64_t initState = 42, uint64_t initSeq = 54);

    uint32_t randomUint32();

    float random() {
        return randomUint32() / static_cast<float>(0xffffffffU);
    }

    float random(float min, float max) {
        return min + random() * (max - min);
    }

    Vec3 randomVersor();
    Vec3 sampleHemisphere(Vec3 n);
    Vec3 sampleHemisphere(Normal3 n);
};



/**
 * @brief Generate an ortho-normal basis from a vector.
 * 
 * From https://graphics.pixar.com/library/OrthonormalB/paper.pdf.
 * "n" must be normalized.
 * 
 * @tparam T 
 * @param n First versor of the base, local z axis, must be normalized.
 * @param b1 Holds the return value of the second versor.
 * @param b2 Holds the return value of the third versor.
 */
template<typename T>
inline void createONB(const T &n, T &b1, T &b2) {
    float sign = copysignf(1.0f, n.z);
    const float a = -1.0f / (sign + n.z);
    const float b = n.x * n.y * a;
    b1 = T(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
    b2 = T(b, sign + n.y * n.y * a, -n.y);
}



#endif