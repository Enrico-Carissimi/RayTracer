#include <iostream>
#include "utils.hpp"
#include "Vec3.hpp"

int main() {
    PCG pcg;
    std::cout << "State: " << pcg.state << "\n";
    std::cout << "Inc:   " << pcg.inc << "\n";

    uint32_t expected[] = {
        2707161783,
        2068313097,
        3122475824,
        2211639955,
        3215226955,
        3421331566,
    };

    for (int i = 0; i < 6; ++i) {
        uint32_t result = pcg.randomUint32();
        std::cout << "Result " << i << ": " << result << "\n";
        sassert(result == expected[i]);
    }

    for (int i = 0; i < 10; ++i) {
        float x = pcg.random(-3.5f, 6.1f);
        sassert(x > -3.5f && x < 6.1f);

        Vec3 v = pcg.randomVersor();
        sassert(areClose(v.norm2(), 1.0f));
        sassert(areClose(pcg.sampleHemisphere(v).norm2(), 1.0f));
        sassert(dot(pcg.sampleHemisphere(v), v) >= 0.0f); // check if the direction is correct
    }

    std::cout << "All tests passed.\n";
    return 0;
}

