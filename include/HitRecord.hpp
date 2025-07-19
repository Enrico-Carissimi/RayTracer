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

/**
 * @struct HitRecord
 * @brief Stores information about a ray-object intersection.
 * 
 * Contains the point of intersection in world coordinates, the surface normal at the hit point,
 * texture coordinates on the surface, the parameter t along the ray where the hit occurred,
 * the ray itself, and a shared pointer to the material of the intersected object.
 * 
 * Also provides a utility function to compare if two HitRecords are approximately equal,
 * considering floating point tolerances.
 */
struct HitRecord {

    Point3 worldPoint;
    Normal3 normal;                   
    Vec2 surfacePoint;
    float t;
    Ray ray;
    std::shared_ptr<Material> material;
    bool isInside = false;

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
