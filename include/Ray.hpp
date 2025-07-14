#ifndef __Ray__
#define __Ray__

#include <limits>
#include "Point3.hpp"
#include "Vec3.hpp"
#include "Transformation.hpp"

/**
 * @brief Represents a ray in 3D space with origin, direction, and valid parameter range.
 */
class Ray {

public: 
    Point3 origin;
    Vec3 direction;
    float tmin, tmax;
    int depth;

    Ray() : origin(Point3()), direction(Vec3(0, 0, 1)), tmin(1e-5), tmax(std::numeric_limits<float>::infinity()), depth(0) {}

    Ray(Point3 origin, Vec3 direction, float tmin = 1e-5, float tmax = std::numeric_limits<float>::infinity(), int depth = 0) : 
    origin(origin), direction(direction), tmin(tmin), tmax(tmax), depth(depth) {}

    bool isClose(const Ray&other, float epsilon = 1e-5f) {
        return origin.isClose(other.origin, epsilon) &&
               direction.isClose(other.direction, epsilon);
    }

    Point3 at (float t) const {
        return origin + direction * t;
    }

    Ray transform(const Transformation& transformation) const {
        return Ray(transformation * origin, transformation * direction, tmin, tmax, depth);
    }
};

#endif