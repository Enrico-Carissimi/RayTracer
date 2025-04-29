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
    // Crea il ray di esempio
    Ray ray(Point3(1.0f, 2.0f, 3.0f), Vec3(6.0f, 5.0f, 4.0f));
    
    // Applichiamo prima la rotazione
    Transformation rotationMatrix = rotation(90.0f, Axis::X);

    // Applichiamo solo la rotazione, senza la traduzione, per il debug
    Ray rotated = ray.transform(rotationMatrix);

    // Stampa il risultato della rotazione
    std::cout << "Rotated origin: " << rotated.origin << std::endl;
    std::cout << "Expected origin after 90° rotation around X: Point3(1.0f, -3.0f, 2.0f)" << std::endl;

    // Ora applichiamo la traduzione
    Transformation translationMatrix = translation(Vec3(10.0f, 11.0f, 12.0f));

    // Trasformiamo il ray con la rotazione e la traduzione
    Ray transformed = rotated.transform(translationMatrix);

    // Stampa il risultato finale della trasformazione
    std::cout << "Transformed origin after translation: " << transformed.origin << std::endl;
    std::cout << "Expected Point3(11.0f, 8.0f, 14.0f)" << std::endl;

    // Esegui le asserzioni per il test
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
