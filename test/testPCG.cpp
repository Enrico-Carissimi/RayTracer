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
        sassert(areClose(randomVersor(pcg).norm2(), 1.0f));
    }

    std::cout << "All tests passed.\n";
    return 0;
}

