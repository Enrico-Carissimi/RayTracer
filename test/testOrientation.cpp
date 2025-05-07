#include <iostream>
#include "../Camera.hpp"
#include <chrono>
#include <thread>

const float aspectRatio = 2.;
const float distance = 1.;

Camera setup() { return Camera("perspective", aspectRatio, 4); }

void testOrientation() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    Camera camera = setup();
    
    Ray topLeftRay = camera.castRay(0, 0, 0., 0.);
    assert(Point3(0., 2., 1.).isClose(topLeftRay.at(1.)));

    Ray bottomRightRay = camera.castRay(3, 1, 1., 1.);
    assert(Point3(0., -2., -1.).isClose(bottomRightRay.at(1.)));

    std::cout << "the image is oriented correctly" << std::endl;
}

int main() {
    testOrientationnn();

    return 0;
}