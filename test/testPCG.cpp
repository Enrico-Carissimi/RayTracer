#include <iostream>
#include "../utils.hpp"
#include <cassert>

int main() {
    rng::PCG pcg;
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
        uint32_t result = pcg.random();
        std::cout << "Result " << i << ": " << result << "\n";
        assert(result == expected[i]);
    }

    std::cout << "All tests passed.\n";
    return 0;
}

