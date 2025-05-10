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
    std::vector<std::shared_ptr<Shape>> shapes;
    //std::vector<PointLight> pointLights;         // Lista di luci

    World() = default;

    void addShape(std::shared_ptr<Shape> shape) {
        shapes.push_back(shape);
    }

    // Metodo per aggiungere una luce al mondo
    //void add_light(const PointLight& light) {
        //point_lights.push_back(light);
    //}

    std::shared_ptr<HitRecord> rayIntersection(const Ray& ray) {
        std::shared_ptr<HitRecord> closest = nullptr;
    
        for (const auto& shape : shapes) {
            HitRecord tempRecord;
            if (shape->isHit(ray, tempRecord)) {
                if (!closest || tempRecord.t < closest->t) {
                    closest = std::make_shared<HitRecord>(tempRecord);
                }
            }
        }
    
        if (closest) {
            closest->normal = closest->normal.normalize();
        }
    
        return closest;
    }
    
    
    bool isPointVisible(const Point3& point, const Point3& observerPos) {
        Vec3 direction = point - observerPos;
        float dirNorm = direction.norm();

        Ray ray(observerPos, direction, 1e-2f / dirNorm, 1.0f);
        for (const auto& shape : shapes) {
            if (shape->quickIsHit(ray)) {
                return false;
            }
        }

        return true;
    }
};

#endif
