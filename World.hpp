#ifndef __World__
#define __World__

#include <vector>
#include <memory>
#include "shapes.hpp"      // Per la classe astratta Shape
#include "Ray.hpp"        // Per il tipo Ray
#include "HitRecord.hpp"  // Per HitRecord
//#include "PointLight.hpp" // Per PointLight
#include "Point3.hpp"      // Per Point (se non l'hai ancora fatto)

class World {
public:
    //std::vector<std::shared_ptr<Shape>> shapes;  // Lista di forme nel mondo
    //std::vector<PointLight> point_lights;         // Lista di luci

    // Costruttore
    World() = default;

    // Metodo per aggiungere una forma al mondo
    void add_shape(std::shared_ptr<Shape> shape) {
        shapes.push_back(shape);
    }

    // Metodo per aggiungere una luce al mondo
    //void add_light(const PointLight& light) {
        //point_lights.push_back(light);
    //}

    // Metodo per determinare se un raggio interseca uno degli oggetti nel mondo
    std::shared_ptr<HitRecord> ray_intersection(const Ray& ray) {
        std::shared_ptr<HitRecord> closest = nullptr;

        for (const auto& shape : shapes) {
            // Verifica l'intersezione con il raggio
            auto intersection = shape->ray_intersection(ray);
            if (!intersection) {
                continue; // Se non c'è intersezione, passa alla forma successiva
            }

            // Se l'intersezione è più vicina di quella precedente, aggiorna
            if (!closest || intersection->t < closest->t) {
                closest = intersection;
            }
        }

        if (closest) {
            closest->normal = closest->normal.normalize();  // Normalizzazione della normale
        }

        return closest;
    }

    // Metodo per verificare se un punto è visibile da un osservatore
    bool is_point_visible(const Point3& point, const Point3& observer_pos) {
        Vec3 direction = point - observer_pos;
        float dir_norm = direction.norm();

        Ray ray(observer_pos, direction, 1e-2f / dir_norm, 1.0f);
        for (const auto& shape : shapes) {
            if (shape->quick_ray_intersection(ray)) {
                return false; // Se c'è una forma che blocca la visibilità, non è visibile
            }
        }

        return true; // Se nessuna forma blocca la visibilità, il punto è visibile
    }
};

#endif // WORLD_HPP
