#include <cassert>
#include <iostream>
#include <cmath>
#include "Ray.hpp"
#include "Point3.hpp"
#include "Vec3.hpp"

void test_is_close() {
    
    Ray ray1(Point3(1.0f, 2.0f, 3.0f), Vec3(5.0f, 4.0f, -1.0f));
    Ray ray2(Point3(1.0f, 2.0f, 3.0f), Vec3(5.0f, 4.0f, -1.0f));
    Ray ray3(Point3(5.0f, 1.0f, 4.0f), Vec3(3.0f, 9.0f, 4.0f));

    assert(ray1.isClose(ray2));  // Ray1 should be close to Ray2
    assert(!(ray1.isClose(ray3))); // Ray1 should NOT be close to Ray3
}

void test_at() {
    Ray ray(Point3(1.0f, 2.0f, 4.0f), Vec3(4.0f, 2.0f, 1.0f));

    assert(ray.at(0.0f).isClose(ray.origin));  // At t=0, it should be the same as the origin
    assert(ray.at(1.0f).isClose(Point3(5.0f, 4.0f, 5.0f)));  // Point at t=1
    assert(ray.at(2.0f).isClose(Point3(9.0f, 6.0f, 6.0f)));  // Point at t=2
}

void test_transform() {
    
    Ray ray(Point3(1.0f, 2.0f, 3.0f), Vec3(6.0f, 5.0f, 4.0f));
    
    Transformation rotationMatrix = rotation(90.0f, Axis::X);

    Ray rotated = ray.transform(rotationMatrix);

    Transformation translationMatrix = translation(Vec3(10.0f, 11.0f, 12.0f));

    Ray transformed = rotated.transform(translationMatrix);
    
    assert(transformed.origin.isClose(Point3(11.0f, 8.0f, 14.0f), 1e-3f));
    assert(transformed.direction.isClose(Vec3(6.0f, -4.0f, 5.0f)));  // Trasformazione della direzione
}



int main() {

    test_is_close();
    test_at();
    test_transform();

    std::cout << "All tests passed!" << std::endl;
    return 0;

}
