#include <iostream>
#include <cassert>
#include <cmath>
#include "../utils.hpp"
#include "../World.hpp"
#include "../materials.hpp"
#include "../Color.hpp"
#include "../Ray.hpp"
#include "../Vec3.hpp"
#include "../renderers.hpp"

int main() {
    PCG pcg;

    for (int i = 0; i < 5; ++i) {
        World world;

        float emittedRadiance = pcg.random();
        float reflectance = pcg.random() * 0.9;

        Color white(1.0f, 1.0f, 1.0f);
        auto emitted = std::make_shared<UniformTexture>(white * emittedRadiance);
        auto texture = std::make_shared<UniformTexture>(white * reflectance);
        auto material = std::make_shared<DiffuseMaterial>(texture, emitted);


        world.addShape(std::make_shared<Sphere>(material));

        Ray ray(Point3(0, 0, 0), Vec3(1, 0, 0));

        Color result = Renderers::PathTracer(ray, world, pcg, 1, 100, 101);

        float expected = emittedRadiance / (1.0f - reflectance);

        std::cout << "Test " << i + 1 << "\n";
        std::cout << "  emitted = " << emittedRadiance << ", reflectance = " << reflectance << "\n";
        std::cout << "  expected = " << expected << ", got = (" << result.r << ", " << result.g << ", " << result.b << ")\n";

        sassert(areClose(result.r, expected, 10e-3));
        sassert(areClose(result.g, expected, 10e-3));
        sassert(areClose(result.b, expected, 10e-3));
    }

    std::cout << "All tests passed!\n";

    return 0;
}