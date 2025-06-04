#include <iostream>
#include <cassert>
#include <cmath>
#include "../utils.hpp"
#include "../Vec3.hpp"

float epsilon = 10e-3;

int main() {
    PCG pcg;

    for (int i = 0; i < 100; ++i) {
        Vec3 normal = Vec3(pcg.random(), pcg.random(), pcg.random()).normalize();

        Vec3 e1, e2, e3;
        e3 = normal;
        createONB(e3, e1, e2);

        sassert(areClose(e3.x, normal.x));
        sassert(areClose(e3.y, normal.y));
        sassert(areClose(e3.z, normal.z));

        sassert(areClose(e1.norm(), 1.0f, epsilon));
        sassert(areClose(e2.norm(), 1.0f, epsilon));
        sassert(areClose(e3.norm(), 1.0f, epsilon));

        sassert(areClose(dot(e1, e2), 0.0f, epsilon));
        sassert(areClose(dot(e2, e3), 0.0f, epsilon));
        sassert(areClose(dot(e3, e1), 0.0f, epsilon));
    }

    std::cout << "Test passed!\n";

    return 0;
}
