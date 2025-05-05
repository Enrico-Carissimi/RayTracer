#ifndef __shapes__
#define __shapes__

#include "Ray.hpp"
#include "HitRecord.hpp"
#include "Transformation.hpp"
#include <cmath>

class Shape {
public:
    Transformation transformation;

    Shape(const Transformation& t = Transformation()) : transformation(t) {}
    virtual ~Shape() = default;

    virtual bool isHit(const Ray& r, HitRecord& rec) const = 0;
};

// Helper to compute normal of a unit sphere
inline Normal3 sphereNormal(const Point3& point, const Vec3& rayDir) {
    Normal3 n(point.x, point.y, point.z);
    return (dot(point.toVec(), rayDir) < 0.0f) ? n : -n;
}

// Helper to compute texture coordinates on unit sphere
inline Vec2 sphereUV(const Point3& point) {
    float u = std::atan2(point.y, point.x) / (2.0f * M_PI);
    float v = std::acos(point.z) / M_PI;
    return Vec2(u, v);
}

class Sphere : public Shape {
    public:
        Sphere(const Transformation& t = Transformation()) : Shape(t) {}
    
        bool isHit(const Ray& r, HitRecord& rec) const override {
            
            Ray invRay = r.transform(transformation.inverse());  
    
            Vec3 originVec = invRay.origin.toVec();
            float a = invRay.direction.norm2();
            float b = 2.0f * dot(originVec, invRay.direction);  
            float c = originVec.norm2() - 1.0f;

            float delta = b * b - 4.0f * a * c;
            if (delta <= 0.0f) return false;  

            float sqrtDelta = std::sqrt(delta);
            float t1 = (-b - sqrtDelta) / (2.0f * a);
            float t2 = (-b + sqrtDelta) / (2.0f * a);
    
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
            rec.worldpoint = transformation * localHit;  
            rec.normal = transformation * sphereNormal(localHit, r.direction);  
            rec.surfacepoint = sphereUV(localHit); 
    
            return true;
        }
    };

#endif
