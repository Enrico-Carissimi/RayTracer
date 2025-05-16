#include <iostream>
#include "../HDRImage.hpp"
#include "../utils.hpp"

using std::cout, std::endl;

bool isClose(float a, float b) {
    return std::fabs(a - b) < 1e-6;
}

int main() {
    HDRImage image(400, 300);

    // check the borders
    sassert(image.validCoordinates(0, 299));
    sassert(image.validCoordinates(399, 0));

    // check for negative coords
    sassert(!image.validCoordinates(-1, 9));
    sassert(!image.validCoordinates(1, -9));

    // check for out of bounds coords
    sassert(!image.validCoordinates(1080, 720));

    // check index calculations
    sassert(image.pixelIndex(1, 2) == 801);
    sassert(image.pixelIndex(11, 203) == 11 + 203 * 400);

    // set and get fail automatically
    image.setPixel(0, 0, Color(1., 1., 1.)); // correct
    bool exceptionThrown = false;
    try{image.getPixel(-2, 0);} // illegal, should get error message
    catch(std::exception& e){exceptionThrown = true;}
    sassert(exceptionThrown);

    // check average luminosity
    image = HDRImage(2, 1); // reset to black
    float a = image.averageLuminosity(); // check the program doesn't crash when all pixels are 0
    image.setPixel(0, 0, Color(5., 10., 15.));
    image.setPixel(1, 0, Color(500., 1000., 1500.));
    sassert(isClose(image.averageLuminosity(0.), 100.));

    // test normalization
    image.normalize(1000., 100.);
    sassert(image.getPixel(0, 0).isClose(Color(0.5e2, 1.0e2, 1.5e2)));
    sassert(image.getPixel(1, 0).isClose(Color(0.5e4, 1.0e4, 1.5e4)));

    // test clamp
    image.clamp();
    for (int i = 0; i < 2; i++) { // check that the rgb values are within the expected boundaries
        auto pixel = image.getPixel(i, 0);
        sassert((pixel.r >= 0) && (pixel.r <= 1));
        sassert((pixel.g >= 0) && (pixel.g <= 1));
        sassert((pixel.b >= 0) && (pixel.b <= 1));
    }
    
    std::cout << "all tests passed" << std::endl;

    return 0;
}