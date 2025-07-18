#ifndef __shapes__
#define __shapes__

#include "Ray.hpp"
#include "HitRecord.hpp"
#include "Transformation.hpp"
#include "utils.hpp"
#include "materials.hpp"

// Helper to compute normal of a unit sphere
inline Normal3 sphereNormal(const Point3& point, const Vec3& rayDir) {
    Normal3 n(point.x, point.y, point.z);
    return (dot(point.toVec(), rayDir) < 0.0f) ? n : -n;
}

// Helper to compute texture coordinates on unit sphere
inline Vec2 sphereUV(const Point3& point) {
    float u = std::atan2(point.y, point.x) / (2.0f * PI);
    u += (u < 0.); // if (u < 0.) u += 1;
    float v = std::acos(std::clamp(point.z, -1.0f, 1.0f)) / PI;
    return Vec2(u, v);
}

class Shape {
public:
    Transformation transformation;

    Shape(std::shared_ptr<Material> material, const Transformation& t = Transformation()) : transformation(t), _material(material) {}
    virtual ~Shape() = default;

    virtual bool isHit(const Ray& r, HitRecord& rec) const = 0;

    virtual bool quickIsHit(const Ray& r) const {
        HitRecord dummy;
        return isHit(r, dummy);
    }

protected:
    std::shared_ptr<Material> _material;
};

/**
 * @brief Sphere shape represented by unit sphere transformed by a transformation.
 */
class Sphere : public Shape {
public:
    Sphere(std::shared_ptr<Material> material = std::make_shared<DiffuseMaterial>(DiffuseMaterial()), const Transformation& t = Transformation()) : Shape(material, t) {}
    
    bool isHit(const Ray& r, HitRecord& rec) const override {
            
        Ray invRay = r.transform(transformation.inverse());
    
        Vec3 originVec = invRay.origin.toVec();
        float a = invRay.direction.norm2();
        float b = dot(originVec, invRay.direction); // actually is b/2
        float c = originVec.norm2() - 1.0f;

        float delta = b * b - a * c; // delta/4
        if (delta <= 0.0f) return false;

        float sqrtDelta = std::sqrt(delta);
        float t1 = (-b - sqrtDelta) / a;
        float t2 = (-b + sqrtDelta) / a;
    
        float t = -1.0f;
        if (t1 > invRay.tmin && t1 < invRay.tmax) {
            t = t1;
        } else if (t2 > invRay.tmin && t2 < invRay.tmax) {
            t = t2;
        } else {
            return false;
        }
    
        Point3 localHit = invRay.at(t);
    
        rec.t = t;
        rec.ray = r;
        rec.worldPoint = transformation * localHit;
        rec.normal = transformation * sphereNormal(localHit, invRay.direction);
        rec.surfacePoint = sphereUV(localHit);
        rec.material = _material;
    
        return true;
    }

    bool quickIsHit(const Ray& ray) const override {
        Ray invRay = ray.transform(transformation.inverse());
        Vec3 origin = invRay.origin.toVec();
        float a = invRay.direction.norm2();
        float b = dot(origin, invRay.direction);
        float c = origin.norm2() - 1.0;

        float delta = b * b - a * c;
        if (delta <= 0.0f) return false;

        float sqrtDelta = std::sqrt(delta);
        float tmin = (-b - sqrtDelta) / a;
        float tmax = (-b + sqrtDelta) / a;

        return (invRay.tmin < tmin && invRay.tmax > tmin) || (invRay.tmin < tmax && invRay.tmax > tmax);
    }
};

/**
 * @brief Plane shape represented as the XY plane at z=0 transformed by a transformation.
 */
class Plane : public Shape {
public:
    Plane(std::shared_ptr<Material> material = std::make_shared<DiffuseMaterial>(DiffuseMaterial()), const Transformation& t = Transformation()) : Shape(material, t) {}

    bool isHit(const Ray& ray, HitRecord& rec) const override {
        Ray invRay = ray.transform(transformation.inverse());

        if (std::abs(invRay.direction.z) < 1e-5f)
            return false;

        float t = -invRay.origin.z / invRay.direction.z;

        if (t <= invRay.tmin || t >= invRay.tmax)
            return false;

        Point3 hitPoint = invRay.at(t);

        rec.worldPoint = transformation * hitPoint;
        rec.normal = transformation * Normal3(0.0f, 0.0f, invRay.direction.z < 0.0f ? 1.0f : -1.0f);
        rec.surfacePoint = Vec2(hitPoint.x - std::floor(hitPoint.x), hitPoint.y - std::floor(hitPoint.y));
        rec.t = t;
        rec.ray = ray;
        rec.material = _material;

        return true;
    }

    bool quickIsHit(const Ray& ray) const override {
        Ray invRay = ray.transform(transformation.inverse());

        if (std::abs(invRay.direction.z) < 1e-5f)
            return false;

        float t = -invRay.origin.z / invRay.direction.z;
        return t > invRay.tmin && t < invRay.tmax;
    }
};

#endif
