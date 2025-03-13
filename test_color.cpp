#include <cassert>
#include <iostream>
#include "Color.hpp"  

int main() {
    Color color(1.0f, 2.0f, 3.0f);

    // First test: must be "close"
    assert(color.isClose(Color(1.0f, 2.0f, 3.0f)));
    std::cout << "Test 1 passed" << std::endl;

    // Second test: must not be "close"
    assert(!color.isClose(Color(3.0f, 4.0f, 5.0f)));
    std::cout << "Test 2 passed" << std::endl;

    // Def new colors
    Color col1(1.0f, 2.0f, 3.0f);
    Color col2(5.0f, 7.0f, 9.0f);

    // Third test: Sum
    assert((col1 + col2).isClose(Color(6.0f, 9.0f, 12.0f)));
    std::cout << "Test 3 (sum) passed" << std::endl;

    // Fourth test: multiply component by component
    assert((col1 * col2).isClose(Color(5.0f, 14.0f, 27.0f)));
    std::cout << "Test 4 (multiply component by component) passed" << std::endl;

    // ----------------------
    // (You can reactivate the test that must fail)
    // ----------------------
    // assert(color.isClose(Color(9.0f, 9.0f, 9.0f)));  // MUST FAIL
    // std::cout << "Test that must fail passed" << std::endl;

    return 0;
}
