#ifndef __shapes__
#define __shapes__

#include "Ray.hpp"
#include "HitRecord.hpp"

class Shape {
    virtual ~Shape() = default;
    virtual bool isHit(const Ray& r, HitRecord& rec) const = 0;
};

class Sphere : Shape {
};

#endif