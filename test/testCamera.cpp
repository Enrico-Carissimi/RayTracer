#include <iostream>
#include "../Camera.hpp"

// we test the functions _castOrthogonal and _castPerspective
// instead of Camera::castRay since the latter uses integer image coordinates
// this is different from prof. Tomasi's implementation

int main() {
    float aspectRatio = 2.;
    float distance = 1.;

// orthogonal projection
    Ray ray1 = _castOrthogonal(0., 0., distance, aspectRatio);
    Ray ray2 = _castOrthogonal(1., 0., distance, aspectRatio);
    Ray ray3 = _castOrthogonal(0., 1., distance, aspectRatio);
    Ray ray4 = _castOrthogonal(1., 1., distance, aspectRatio);

    // verify that the rays are parallel by verifying that cross-products vanish
    sassert(areClose(0., cross(ray1.direction, ray2.direction).norm2()));
    sassert(areClose(0., cross(ray1.direction, ray3.direction).norm2()));
    sassert(areClose(0., cross(ray1.direction, ray4.direction).norm2()));

    // verify that the ray hitting the corners have the right coordinates
    sassert(ray1.at(1.).isClose(Point3(0., 2., -1.)));
    sassert(ray2.at(1.).isClose(Point3(0., -2., -1.)));
    sassert(ray3.at(1.).isClose(Point3(0., 2., 1.)));
    sassert(ray4.at(1.).isClose(Point3(0., -2., 1.)));

    std::cout << "orthogonal projection works" << std::endl;



// perspective projection
    ray1 = _castPerspective(0., 0., distance, aspectRatio);
    ray2 = _castPerspective(1., 0., distance, aspectRatio);
    ray3 = _castPerspective(0., 1., distance, aspectRatio);
    ray4 = _castPerspective(1., 1., distance, aspectRatio);

    // verify that all the rays depart from the same point
    sassert(ray1.origin.isClose(ray2.origin));
    sassert(ray1.origin.isClose(ray3.origin));
    sassert(ray1.origin.isClose(ray4.origin));

    // verify that the ray hitting the corners have the right coordinates
    sassert(ray1.at(1.).isClose(Point3(0., 2., -1.)));
    sassert(ray2.at(1.).isClose(Point3(0., -2., -1.)));
    sassert(ray3.at(1.).isClose(Point3(0., 2., 1.)));
    sassert(ray4.at(1.).isClose(Point3(0., -2., 1.)));

    std::cout << "perspective projection works" << std::endl;


// transform camera (adapted, same as above)
    Camera camera("orthogonal", aspectRatio, 200, 1., translation(-Vec3(0., 1., 0.) * 2.) * rotation(90, Axis::Z));
    Ray ray = camera.castRay(100, 50, -0.5, -0.5); // will need to be changed to (100, 50, 0., 0.) when we fix the (intended) error in Camera
    sassert(ray.at(1.).isClose(Point3(0., -2., 0.)));

    std::cout << "transform works" << std::endl;



// "ImageTracer" or castAll
    camera = Camera("perspective", aspectRatio, 4);

    ray1 = camera.castRay(0, 0, 2.5, 1.5);
    ray2 = camera.castRay(2, 1, 0.5, 0.5);
    sassert(ray1.isClose(ray2));

    camera.castAll([](Ray){ return Color(1., 2., 3.); });

    for (int row = 0; row < camera.imageHeight; row++) {
        for (int col = 0; col < camera.imageWidth; col++) {
            sassert(camera.image.getPixel(col, row).isClose(Color(1., 2., 3.)));
        }
    }

    std::cout << "castAll works" << std::endl;

    return 0;
}