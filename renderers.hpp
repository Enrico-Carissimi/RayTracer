#include "World.hpp"

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

}