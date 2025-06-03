#include "World.hpp"
#include <algorithm>

namespace Renderers {

auto OnOff = [](Ray ray, const World& world) {
    HitRecord rec;
    return world.isHit(ray, rec) ? Color(1., 1., 1.) : Color(0., 0., 0.);
};

auto Flat = [](Ray ray, const World& world) {
    HitRecord rec;
    return world.isHit(ray, rec) ? rec.material->eval(rec.surfacePoint) : Color(0., 0., 0.);
};

auto PointLight = [](Ray ray, const World& world,
                     Color backgroundColor = Color(0., 0., 0.),
                     Color ambientColor = Color(0.1, 0.1, 0.1)) {
    HitRecord hit;
    if (!world.isHit(ray, hit))
        return backgroundColor;

    Color resultColor = ambientColor;

    for (const auto& light : world.pointLights) {
        if (const_cast<World&>(world).isPointVisible(light.position, hit.worldPoint)) {
            Vec3 distanceVec = hit.worldPoint - light.position;
            float distance = distanceVec.norm();
            Vec3 inDir = distanceVec * (1.0f / distance); // normalize

            float cosTheta = std::max(0.0f, dot(-ray.direction.normalize(), hit.normal.normalize()));
            float distanceFactor = (light.linearRadius > 0.0f)
                                     ? std::pow(light.linearRadius / distance, 2.0f)
                                     : 1.0f;

            Color emitted = hit.material->emittedRadiance(hit.surfacePoint);
            Color brdf = hit.material->eval(hit.surfacePoint);

            resultColor = resultColor + (emitted + brdf) * light.color * cosTheta * distanceFactor;
        }
    }

    return resultColor;
};


}


