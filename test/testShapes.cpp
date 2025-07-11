#include <iostream>
#include "shapes.hpp"
#include "World.hpp"

using std::cout, std::endl;

auto bufferMaterial = std::make_shared<DiffuseMaterial>(DiffuseMaterial());

// tests for sphere
namespace sphere {

void testHit() {
    Sphere sphere;
    HitRecord rec;

    Ray ray1(Point3(0., 0., 2.), -Vec3(0., 0., 1.));
    sassert(sphere.isHit(ray1, rec));

    // HitRecord doesn't have a constructor and I don't think it should
    HitRecord rec1;
    rec1.worldPoint = Point3(0., 0., 1.), rec1.normal = Normal3(0., 0., 1.), rec1.ray = ray1, rec1.surfacePoint = Vec2(0., 0.), rec1.t = 1.;
    sassert(rec1.isClose(rec));

    Ray ray2(Point3(3., 0., 0.), -Vec3(1., 0., 0.));
    sassert(sphere.isHit(ray2, rec));

    HitRecord rec2;
    rec2.worldPoint = Point3(1., 0., 0.), rec2.normal = Normal3(1., 0., 0.), rec2.ray = ray2, rec2.surfacePoint = Vec2(0., 0.5), rec2.t = 2.;
    sassert(rec2.isClose(rec));

    sassert(!sphere.isHit(Ray(Point3(0., 10., 2.), -Vec3(0., 0., 1.)), rec));

    cout << "isHit works" << endl;
}

void testInnerHit() {
    Sphere sphere;
    HitRecord rec;

    Ray ray(Point3(0., 0., 0.), Vec3(1., 0., 0.));
    sassert(sphere.isHit(ray, rec));

    HitRecord other;
    other.worldPoint = Point3(1., 0., 0.), other.normal = Normal3(-1., 0., 0.), other.surfacePoint = Vec2(0., 0.5), other.t = 1., other.ray = ray;
    sassert(rec.isClose(other));

    cout << "inner hit is handled correctly" << endl;
}

void testTransformation() {
    Sphere sphere(bufferMaterial, translation(Vec3(10., 0., 0.)));
    HitRecord rec;

    Ray ray1 = Ray(Point3(10., 0., 2.), -Vec3(0., 0., 1.));
    sassert(sphere.isHit(ray1, rec));
    HitRecord rec1;
    rec1.worldPoint = Point3(10., 0., 1.), rec1.normal = Normal3(0., 0., 1.), rec1.surfacePoint = Vec2(0., 0.), rec1.t = 1., rec1.ray = ray1;
    sassert(rec1.isClose(rec));

    Ray ray2(Point3(13., 0., 0.), -Vec3(1., 0., 0));
    sassert(sphere.isHit(ray2, rec));
    HitRecord rec2;
    rec2.worldPoint = Point3(11., 0., 0.), rec2.normal = Normal3(1., 0., 0.), rec2.surfacePoint = Vec2(0., 0.5), rec2.t = 2., rec2.ray = ray2;
    sassert(rec2.isClose(rec));

    // check if the sphere failed to move by trying to hit the untransformed shape
    sassert(!sphere.isHit(Ray(Point3(0., 0., 2.), -Vec3(0., 0., 1.)), rec));

    // check if the *inverse* transformation was wrongly applied
    sassert(!sphere.isHit(Ray(Point3(-10., 0., 0.), -Vec3(0., 0., 1.)), rec));

    cout << "transformations work" << endl;
}

void testNormals() {
    Sphere sphere(bufferMaterial, scaling(Vec3(2., 1., 1.)));
    Ray ray(Point3(1., 1., 0.), Vec3(-1., -1., 0.));
    HitRecord rec;
    sphere.isHit(ray, rec);
    // we normalize "rec.normal", as we are not interested in its length
    sassert(rec.normal.normalize().isClose(Normal3(1., 4., 0.).normalize()));

    cout << "normals are handled correctly" << endl;
}

void testNormalDirection() {
    // scaling a sphere by -1 keeps the sphere the same but reverses its reference frame
    Sphere sphere(bufferMaterial, scaling(Vec3(-1., -1., -1.)));
    Ray ray(Point3(0., 2., 0.), -Vec3(0., 1., 0.));
    HitRecord rec;
    sphere.isHit(ray, rec);
    // we normalize "rec.normal", as we are not interested in its length
    sassert(rec.normal.normalize().isClose(Normal3(0., 1., 0.).normalize()));

    cout << "normals direction is handled correctly" << endl;
}

void testUVCoordinates() {
    Sphere sphere;

    // The first four rays hit the unit sphere at the
    // points P1, P2, P3, and P4.
    //
    //                    ^ y
    //                    | P2
    //              , - ~ * ~ - ,
    //          , '       |       ' ,
    //        ,           |           ,
    //       ,            |            ,
    //      ,             |             , P1
    // -----*-------------+-------------*---------> x
    //   P3 ,             |             ,
    //       ,            |            ,
    //        ,           |           ,
    //          ,         |        , '
    //            ' - , _ * _ ,  '
    //                    | P4
    //
    // P5 and P6 are aligned along the x axis and are displaced
    // along z (ray5 in the positive direction, ray6 in the negative
    // direction).

    HitRecord rec;

    Ray ray1(Point3(2., 0., 0.), -Vec3(1., 0., 0.));
    sphere.isHit(ray1, rec);
    sassert(rec.surfacePoint.isClose(Vec2(0., 0.5)));

    Ray ray2(Point3(0., 2., 0.), -Vec3(0., 1., 0.));
    sphere.isHit(ray2, rec);
    sassert(rec.surfacePoint.isClose(Vec2(0.25, 0.5)));

    Ray ray3(Point3(-2., 0., 0.), Vec3(1., 0., 0.));
    sphere.isHit(ray3, rec);
    sassert(rec.surfacePoint.isClose(Vec2(0.5, 0.5)));

    Ray ray4(Point3(0., -2., 0.), Vec3(0., 1., 0.));
    sphere.isHit(ray4, rec);
    sassert(rec.surfacePoint.isClose(Vec2(0.75, 0.5)));

    Ray ray5(Point3(2., 0., 0.5), -Vec3(1., 0., 0.));
    sphere.isHit(ray5, rec);
    sassert(rec.surfacePoint.isClose(Vec2(0., 1. / 3.)));

    Ray ray6(Point3(2., 0., -0.5), -Vec3(1., 0., 0.));
    sphere.isHit(ray6, rec);
    sassert(rec.surfacePoint.isClose(Vec2(0., 2. / 3.)));

    cout << "surface coordinates are handled correctly" << endl;
}

}



// tests for plane
namespace plane {

void testHit() {
    Plane plane;
    HitRecord rec;

    Ray ray1(Point3(0., 0., 1.), -Vec3(0., 0., 1.));
    sassert(plane.isHit(ray1, rec));
    HitRecord rec1;
    rec1.worldPoint = Point3(0., 0., 0.), rec1.normal = Normal3(0., 0., 1.), rec1.surfacePoint = Vec2(0., 0.), rec1.t = 1., rec1.ray = ray1;
    sassert(rec1.isClose(rec));

    Ray ray2(Point3(0., 0., 1.), Vec3(0., 0., 1.));
    sassert(!plane.isHit(ray2, rec));

    Ray ray3(Point3(0., 0., 1.), Vec3(1., 0., 0.));
    sassert(!plane.isHit(ray3, rec));

    Ray ray4(Point3(0., 0., 1.), Vec3(0., 1., 0.));
    sassert(!plane.isHit(ray4, rec));

    cout << "isHit works" << endl;
}

void testTransformation() {
    Plane plane(bufferMaterial, rotation(90., Axis::Y));
    HitRecord rec;

    Ray ray1(Point3(1., 0., 0.), -Vec3(1., 0., 0.));
    sassert(plane.isHit(ray1, rec));
    HitRecord rec1;
    rec1.worldPoint = Point3(0., 0., 0.), rec1.normal = Normal3(1., 0., 0.), rec1.surfacePoint = Vec2(0., 0.), rec1.t = 1., rec1.ray = ray1;
    sassert(rec1.isClose(rec));

    Ray ray2(Point3(0., 0., 1.), Vec3(0., 0., 1.));
    sassert(!plane.isHit(ray2, rec));

    Ray ray3(Point3(0., 0., 1.), Vec3(1., 0., 0.));
    sassert(!plane.isHit(ray2, rec));

    Ray ray4(Point3(0., 0., 1.), Vec3(0., 1., 0.));
    sassert(!plane.isHit(ray2, rec));

    cout << "transformations work" << endl;
}

void testUVCoordinates() {
    Plane plane;
    HitRecord rec;

    Ray ray1(Point3(0., 0., 1.), -Vec3(0., 0., 1.));
    plane.isHit(ray1, rec);
    sassert(rec.surfacePoint.isClose(Vec2(0., 0.)));

    Ray ray2(Point3(0.25, 0.75, 1.), -Vec3(0., 0., 1.));
    plane.isHit(ray2, rec);
    sassert(rec.surfacePoint.isClose(Vec2(0.25, 0.75)));

    Ray ray3(Point3(4.25, 7.75, 1.), -Vec3(0., 0., 1.));
    plane.isHit(ray3, rec);
    sassert(rec.surfacePoint.isClose(Vec2(0.25, 0.75)));

    cout << "surface coordinates are handled correctly" << endl;
}

}



namespace world {

auto testHit() {
    World world;
    HitRecord rec;

    Sphere sphere1(bufferMaterial, translation(2., 0., 0.));
    Sphere sphere2(bufferMaterial, translation(8., 0., 0.));
    world.addShape(std::make_shared<Sphere>(sphere1));
    world.addShape(std::make_shared<Sphere>(sphere2));

    sassert(world.isHit(Ray(Point3(0., 0., 0.), Vec3(1., 0., 0.)), rec));
    sassert(rec.worldPoint.isClose(Point3(1., 0., 0.)));

    sassert(world.isHit(Ray(Point3(10., 0., 0.), -Vec3(1., 0., 0.)), rec));
    sassert(rec.worldPoint.isClose(Point3(9., 0., 0.)));

    cout << "isHit works" << endl;
}

void testQuickHit() {
    World world;

    Sphere sphere1(bufferMaterial, translation(2., 0., 0.));
    Sphere sphere2(bufferMaterial, translation(8., 0., 0.));
    world.addShape(std::make_shared<Sphere>(sphere1));
    world.addShape(std::make_shared<Sphere>(sphere2));

    sassert(!world.isPointVisible(Point3(10., 0., 0.), Point3(0., 0., 0.)));
    sassert(!world.isPointVisible(Point3(5., 0., 0.), Point3(0., 0., 0.)));
    sassert(world.isPointVisible(Point3(5., 0., 0.), Point3(4., 0., 0.)));
    sassert(world.isPointVisible(Point3(0.5, 0., 0.), Point3(0., 0., 0.)));
    sassert(world.isPointVisible(Point3(0., 10., 0.), Point3(0., 0., 0.)));
    sassert(world.isPointVisible(Point3(0., 0., 10.), Point3(0., 0., 0.)));

    cout << "quick hit works" << endl;
}

}



int main() {
    // sphere
    cout << "Sphere:" << endl;
    sphere::testHit();
    sphere::testInnerHit();
    sphere::testTransformation();
    sphere::testNormals();
    sphere::testNormalDirection();
    sphere::testUVCoordinates();

    // plane
    cout << "\nPlane:" << endl;
    plane::testHit();
    plane::testTransformation();
    plane::testUVCoordinates();

    // world
    cout << "\nWorld:" << endl;
    world::testHit();
    world::testQuickHit();

    return 0;
}