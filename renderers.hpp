#include "World.hpp"
#include <algorithm>

namespace Renderers {

auto OnOff = [](const Ray& ray, const World& world) {
    HitRecord rec;
    return world.isHit(ray, rec) ? Color(1., 1., 1.) : Color(0., 0., 0.);
};

auto Flat = [](const Ray& ray, const World& world) {
    HitRecord rec;
    return world.isHit(ray, rec) ? rec.material->color(rec.surfacePoint) : Color(0., 0., 0.);
};

auto PathTracer = [](const Ray& ray, const World& world, PCG& pcg, int nRays = 8,
                     int maxDepth = 8, int russianRouletteLimit = 3) -> Color {

    // we need to do this to use recursion without passing the lambda to itself in the parameters of Camera::render
    auto actualFunction = [](const auto& self, const Ray& ray, const World& world, PCG& pcg, int nRays, // ew
                             int maxDepth, int russianRouletteLimit) -> Color {

        if (ray.depth > maxDepth) { return Color(0., 0., 0.); }

        HitRecord rec;
        if (!world.isHit(ray, rec)) { return world.backgroundColor; }

        auto hitMaterial = rec.material;
        Color hitColor = hitMaterial->color(rec.surfacePoint);
        Color emittedRadiance = hitMaterial->emittedColor(rec.surfacePoint);

        float hitColorLuminosity = std::max({hitColor.r, hitColor.g, hitColor.b});

        // russian roulette
        if (ray.depth >= russianRouletteLimit) {
            float q = std::max(0.05, 1. - hitColorLuminosity);
            if (pcg.random() > q) {
                // keep the recursion going, but compensate for other potentially discarded rays
                hitColor *= 1. / (1. - q);
            } else {
                // terminate prematurely
                return emittedRadiance;
            }
        }

        Color totalRadiance;
        if (hitColorLuminosity > 0.) {  // only do costly recursions if it's worth it
            for (int i = 0; i < nRays; i++) {
                Ray newRay = hitMaterial->scatterRay(pcg, rec.ray.direction, rec.worldPoint, rec.normal, ray.depth + 1);
                Color newRadiance = self(self, newRay, world, pcg, nRays, maxDepth, russianRouletteLimit); // recursive call
                totalRadiance += hitColor * newRadiance;
            }
        }
        
        return emittedRadiance + totalRadiance * (1. / nRays); // no operator / in Color
    };

    return actualFunction(actualFunction, ray, world, pcg, nRays, maxDepth, russianRouletteLimit);
};

auto PointLight = [](const Ray& ray, const World& world,
                     Color ambientColor = Color(0.1, 0.1, 0.1)) {
    HitRecord hit;
    if (!world.isHit(ray, hit))
        return world.backgroundColor;


    Color emitted = hit.material->emittedColor(Vec2(0.0f, 0.0f));
    Color resultColor = ambientColor + emitted;

    for (const auto& light : world.pointLights) {
        if (world.isPointVisible(light.position, hit.worldPoint)) {
            Vec3 distanceVec = hit.worldPoint - light.position;
            float distance = distanceVec.norm();
            Vec3 inDir = distanceVec / distance; // normalize

            float cosTheta = std::max(0.0f, dot(hit.normal.normalize(), -inDir));

            float distanceFactor = (light.linearRadius > 0.0f)
                                     ? (light.linearRadius / distance) * (light.linearRadius / distance)
                                     : 1.0f;

            Vec3 inDir2 = (light.position - hit.worldPoint).normalize();
            Vec3 outDir = -ray.direction.normalize();

            float thetaIn = std::acos(std::clamp(dot(hit.normal, inDir2), -1.0f, 1.0f));
            float thetaOut = std::acos(std::clamp(dot(hit.normal, outDir), -1.0f, 1.0f));

            Color brdf = hit.material->eval(Vec2(0.0f, 0.0f), thetaIn, thetaOut);

            resultColor += brdf * light.color * cosTheta * distanceFactor;
        }
    }

    return resultColor;
};


}


