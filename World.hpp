#ifndef __World__
#define __World__

#include <vector>
#include <memory>
#include "shapes.hpp"
#include "Ray.hpp"
#include "HitRecord.hpp"
//#include "PointLight.hpp"
#include "Point3.hpp"

class World {
public:
    Color backgroundColor;
    //std::vector<PointLight> pointLights;         // Lista di luci

    World() = default;

    void addShape(std::shared_ptr<Shape> shape) {
        _shapes.push_back(shape);
    }

    // Metodo per aggiungere una luce al mondo
    //void add_light(const PointLight& light) {
        //point_lights.push_back(light);
    //}

    bool isHit(const Ray& ray, HitRecord& rec) const {
        bool hit = false;
        float closest = ray.tmax;
    
        for (const auto& shape : _shapes) {
            HitRecord tempRecord;
            if (shape->isHit(ray, tempRecord)) {
                if (!hit || tempRecord.t < closest) { // if the ray is not const, we could avoid this if by doing ray.tmax = closest
                    hit = true;
                    closest = tempRecord.t;
                    rec = tempRecord;
                }
            }
        }
    
        if (hit) {
            rec.normal = rec.normal.normalize();
        }
    
        return hit;
    }
    
    
    bool isPointVisible(const Point3& point, const Point3& observerPos) {
        Vec3 direction = point - observerPos;
        float dirNorm = direction.norm();

        Ray ray(observerPos, direction, 1e-2f / dirNorm, 1.0f);
        for (const auto& shape : _shapes) {
            if (shape->quickIsHit(ray)) {
                return false;
            }
        }

        return true;
    }

private:
    std::vector<std::shared_ptr<Shape>> _shapes;
};

#endif
