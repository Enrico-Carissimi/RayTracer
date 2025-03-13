#include <iostream>
#include <cassert>
#include "HDRImage.hpp"

int main() {
    HDRImage image(400, 300);

    // check the borders
    assert(image.validCoordinates(0, 299));
    assert(image.validCoordinates(399, 0));

    // check for negative coords
    assert(!image.validCoordinates(-1, 9));
    assert(!image.validCoordinates(1, -9));

    // check for out of bounds coords
    assert(!image.validCoordinates(1080, 720));

    // check index calculations
    assert(image.pixelIndex(1, 2) == 801);
    assert(image.pixelIndex(11, 203) == 11 + 203 * 400);

    std::cout << "all tests passed" << std::endl;

    // set and get fail automatically
    image.setPixel(0, 0, Color(1., 1., 1.)); // correct
    image.getPixel(-2, 0); // illegal, should get error message

    return 0;
}