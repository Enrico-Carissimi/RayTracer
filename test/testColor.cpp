#include <iostream>
#include "../Color.hpp"
#include "../utils.hpp"

int main() {
    Color color(1.0f, 2.0f, 3.0f);

    // First test: must be "close"
    sassert(color.isClose(Color(1.0f, 2.0f, 3.0f)));
    std::cout << "Test 1 passed" << std::endl;
    // First test (negative): must NOT be "close"
    sassert(!color.isClose(Color(3.0f, 4.0f, 5.0f)));
    std::cout << "Test 1 (negative) passed" << std::endl;

    // Define new colors
    Color col1(1.0f, 2.0f, 3.0f);
    Color col2(5.0f, 7.0f, 9.0f);

    // Second test: Sum
    sassert((col1 + col2).isClose(Color(6.0f, 9.0f, 12.0f)));
    std::cout << "Test 2 (sum) passed" << std::endl;
    // Second test (negative)
    sassert(!( (col1 + col2).isClose(Color(0.0f, 0.0f, 0.0f)) ));
    std::cout << "Test 2 (sum negative) passed" << std::endl;

    // Third test: multiply component by component
    sassert((col1 * col2).isClose(Color(5.0f, 14.0f, 27.0f)));
    std::cout << "Test 3 (multiply component-wise) passed" << std::endl;
    // Third test (negative)
    sassert(!( (col1 * col2).isClose(Color(1.0f, 1.0f, 1.0f)) ));
    std::cout << "Test 3 (multiply component-wise negative) passed" << std::endl;

    // Fourth test: scalar multiplication by 2.0
    Color result = col1 * 2.0f;
    sassert(result.isClose(Color(2.0f, 4.0f, 6.0f)));
    std::cout << "Test 4 (multiply by 2.0) passed" << std::endl;
    // Fourth test (negative)
    sassert(!(result.isClose(Color(9.0f, 9.0f, 9.0f))));
    std::cout << "Test 4 (multiply by 2.0 negative) passed" << std::endl;

    // Fifth test: scalar multiplication by 0.5
    Color result2 = col1 * 0.5f;
    sassert(result2.isClose(Color(0.5f, 1.0f, 1.5f)));
    std::cout << "Test 5 (multiply by 0.5) passed" << std::endl;
    // Fifth test (negative)
    sassert(!(result2.isClose(Color(0.0f, 0.0f, 0.0f))));
    std::cout << "Test 5 (multiply by 0.5 negative) passed" << std::endl;

    // Sixth test: scalar multiplication by 0
    Color result3 = col1 * 0.0f;
    sassert(result3.isClose(Color(0.0f, 0.0f, 0.0f)));
    std::cout << "Test 6 (multiply by 0) passed" << std::endl;
    // Sixth test (negative)
    sassert(!(result3.isClose(Color(1.0f, 2.0f, 3.0f))));
    std::cout << "Test 6 (multiply by 0 negative) passed" << std::endl;

    // Seventh test: scalar multiplication by negative number
    Color result4 = col1 * (-1.0f);
    sassert(result4.isClose(Color(-1.0f, -2.0f, -3.0f)));
    std::cout << "Test 7 (multiply by -1.0) passed" << std::endl;
    // Seventh test (negative)
    sassert(!(result4.isClose(Color(1.0f, 2.0f, 3.0f))));
    std::cout << "Test 7 (multiply by -1.0 negative) passed" << std::endl;

    return 0;
}

