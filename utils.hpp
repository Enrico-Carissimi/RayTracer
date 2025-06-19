#ifndef __utils__
#define __utils__

#include <iostream>
#include <source_location>
#include <cmath>
#include <string>
#include <cstdint>
#include <limits>

#define PI 3.1415926535897932385
const float INF = std::numeric_limits<float>::infinity();



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

// utilities for the lexer
bool isCharSkippable(const char& c) {
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '#');
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

/**
 * @brief tests if an exception is thrown by a function with the specified parameter
 * 
 * if the function accepts more parameters, or if it is a constructor, use a lambda:
 * testException(parameter, [](Parameter p) -> {return function(p, [...]);});
 * 
 * @tparam Parameter 
 * @tparam Function 
 * @param parameter parameter to pass to the function
 * @param function the function throwing the exeption to test
 */
template<typename Parameter, typename Function>
void testException(Parameter& parameter, Function function) {
    bool exceptionThrown = false;
    try{function(parameter);}
    catch(std::exception& e){exceptionThrown = true;}
    sassert(exceptionThrown);
}



class PCG {
public:
    uint64_t state;
    uint64_t inc;

    PCG(uint64_t initState = 42, uint64_t initSeq = 54) {
        state = 0;
        inc = (initSeq << 1u) | 1u;
        random();
        state = (state + initState);
        random();
    }

    uint32_t randomUint32() {
        uint64_t oldState = state;
        state = static_cast<uint64_t>(oldState * 6364136223846793005ULL + inc);
        uint32_t xorshifted = static_cast<uint32_t>(((oldState >> 18u) ^ oldState) >> 27u);
        uint32_t rot = static_cast<uint32_t>(oldState >> 59u);
        return (((xorshifted >> rot) | (xorshifted << ((-rot) & 31))));
    }

    float random() {

        return randomUint32() / static_cast<float>(0xffffffffU);

    }
};



// from https://graphics.pixar.com/library/OrthonormalB/paper.pdf
// n must be normalized
template<typename T>
void createONB(const T &n, T &b1, T &b2) {
    float sign = copysignf(1.0f, n.z);
    const float a = -1.0f / (sign + n.z);
    const float b = n.x * n.y * a;
    b1 = T(1.0f + sign * n.x * n.x * a, sign * b, -sign * n.x);
    b2 = T(b, sign + n.y * n.y * a, -n.y);
}



#endif