#ifndef __HitRecord__
#define __HitRecord__

#include "Vec3.hpp"
#include "Point3.hpp"
#include "Normal3.hpp"
#include "Vec2.hpp"
#include "Ray.hpp"
#include <memory>
#include <cmath>

class Material;

struct HitRecord {

    Point3 worldPoint;
    Normal3 normal;                   
    Vec2 surfacePoint;
    float t;
    Ray ray;
    std::shared_ptr<Material> material;

    HitRecord() = default;

    bool isClose(const HitRecord& other, float epsilon = 1e-5f) {
        return worldPoint.isClose(other.worldPoint, epsilon) &&
               normal.isClose(other.normal, epsilon) &&
               surfacePoint.isClose(other.surfacePoint, epsilon) &&
               areClose(t, other.t, epsilon) &&
               ray.isClose(other.ray, epsilon);
    }
    
};

#endif
