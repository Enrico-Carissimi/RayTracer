#ifndef __renderers__
#define __renderers__

#include "World.hpp"
#include <algorithm>

namespace Renderers {

/**
 * @brief Simple on/off renderer: returns white if the ray hits anything, black otherwise.
 * 
 * @param ray Ray to test for intersection.
 * @param world Scene containing objects.
 * @return Color White if hit, black otherwise.
 */
auto OnOff = [](const Ray& ray, const World& world) {
    HitRecord rec;
    return world.isHit(ray, rec) ? Color(1.0f, 1.0f, 1.0f) : Color(0.0f, 0.0f, 0.0f);
};

/**
 * @brief Flat renderer: returns the flat color of the material if hit, otherwise background color.
 * 
 * @param ray Ray to test for intersection.
 * @param world Scene containing objects.
 * @return Color Material color at hit point or background color.
 */
auto Flat = [](const Ray& ray, const World& world) {
    HitRecord rec;
    return world.isHit(ray, rec) ? rec.material->color(rec.surfacePoint) : world.backgroundColor;
};

/**
 * @brief Path tracer renderer using recursive Monte Carlo integration with Russian roulette termination.
 * 
 * @param ray Ray to trace.
 * @param world Scene containing objects and lights.
 * @param pcg Pseudo-random number generator.
 * @param nRays Number of rays per bounce for Monte Carlo sampling (default 8).
 * @param maxDepth Maximum recursion depth (default 8).
 * @param russianRouletteLimit Recursion depth after which Russian roulette is applied (default 3).
 * @return Color Computed radiance along the ray.
 */
auto PathTracer = [](const Ray& ray, const World& world, PCG& pcg, int nRays = 8,
                     int maxDepth = 8, int russianRouletteLimit = 3) -> Color {

    // we need to do this to use recursion without passing the lambda to itself in the parameters of Camera::render
    auto actualFunction = [](const auto& self, const Ray& ray, const World& world, PCG& pcg, int nRays, // ew
                             int maxDepth, int russianRouletteLimit) -> Color {

        if (ray.depth > maxDepth) { return Color(0.0f, 0.0f, 0.0f); }

        HitRecord rec;
        if (!world.isHit(ray, rec)) { return world.backgroundColor; }

        auto hitMaterial = rec.material;
        Color hitColor = hitMaterial->color(rec.surfacePoint);
        Color emittedRadiance = hitMaterial->emittedColor(rec.surfacePoint);

        float hitColorLuminosity = std::max({hitColor.r, hitColor.g, hitColor.b});

        // russian roulette
        if (ray.depth >= russianRouletteLimit) {
            float q = std::max(0.05f, 1.0f - hitColorLuminosity);
            if (pcg.random() > q) {
                // keep the recursion going, but compensate for other potentially discarded rays
                hitColor *= 1.0f / (1.0f - q);
            } else {
                // terminate prematurely
                return emittedRadiance;
            }
        }

        Color totalRadiance;
        if (hitColorLuminosity > 0.0f) {  // only do costly recursions if it's worth it
            for (int i = 0; i < nRays; i++) {
                Ray newRay = hitMaterial->scatterRay(pcg, rec, ray.depth + 1);
                Color newRadiance = self(self, newRay, world, pcg, nRays, maxDepth, russianRouletteLimit); // recursive call
                totalRadiance += hitColor * newRadiance;
            }
        }
        
        return emittedRadiance + totalRadiance * (1.0f / nRays); // no operator / in Color
    };

    return actualFunction(actualFunction, ray, world, pcg, nRays, maxDepth, russianRouletteLimit);
};

/**
 * @brief Simple point light renderer combining ambient lighting and direct illumination from point lights.
 * 
 * @param ray Ray to trace.
 * @param world Scene containing objects and lights.
 * @param ambientColor Ambient light color added to the final result (default: low gray).
 * @return Color Computed color including ambient and direct lighting.
 */
auto PointLight = [](const Ray& ray, const World& world,
                     const Color& ambientColor = Color(0.1f, 0.1f, 0.1f)) {
    HitRecord hit;
    if (!world.isHit(ray, hit))
        return world.backgroundColor;

    Color emitted = hit.material->emittedColor(Vec2(0.0f, 0.0f));
    Color resultColor = ambientColor + emitted;

    for (const auto& light : world.pointLights) {
        if (!world.isPointVisible(light.position, hit.worldPoint))
            continue;

        Vec3 distanceVec = hit.worldPoint - light.position;
        float distance = distanceVec.norm();
        Vec3 inDir = distanceVec / distance; // normalize

        float cosTheta = std::max(0.0f, dot(hit.normal.normalize(), -inDir));

        float distanceFactor = (light.linearRadius > 0.0f)
                               ? (light.linearRadius / distance) * (light.linearRadius / distance)
                               : 1.0f;

        Vec3 inDir2 = (light.position - hit.worldPoint).normalize();
        Vec3 outDir = -ray.direction.normalize();

        float thetaIn = std::acos(dot(hit.normal.normalize(), inDir2));
        float thetaOut = std::acos(dot(hit.normal.normalize(), outDir));

        Color brdf = hit.material->eval(Vec2(hit.surfacePoint), thetaIn, thetaOut);

        resultColor += brdf * light.color * cosTheta * distanceFactor;
    }

    return resultColor;
};

}

#endif