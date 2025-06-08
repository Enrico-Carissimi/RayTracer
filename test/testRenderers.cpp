#include "../Camera.hpp"
#include "../renderers.hpp"
#include "../utils.hpp"

const Color BLACK(0., 0., 0.);
const Color WHITE(1., 1., 1.);

void testOnOffRenderer() {
    auto material = std::make_shared<DiffuseMaterial>(std::make_shared<UniformTexture>(WHITE));
    Sphere sphere(material, translation(Vec3(2., 0., 0.)) * scaling(Vec3(0.2, 0.2, 0.2)));

    Camera camera("orthogonal", 1., 3.);
    World world;
    world.addShape(std::make_shared<Sphere>(sphere));
    camera.render(Renderers::OnOff, world);

    sassert(camera.image.getPixel(0, 0).isClose(BLACK));
    sassert(camera.image.getPixel(1, 0).isClose(BLACK));
    sassert(camera.image.getPixel(2, 0).isClose(BLACK));

    sassert(camera.image.getPixel(0, 1).isClose(BLACK));
    sassert(camera.image.getPixel(1, 1).isClose(WHITE));
    sassert(camera.image.getPixel(2, 1).isClose(BLACK));

    sassert(camera.image.getPixel(0, 2).isClose(BLACK));
    sassert(camera.image.getPixel(1, 2).isClose(BLACK));
    sassert(camera.image.getPixel(2, 2).isClose(BLACK));

    std::cout << "on-off renderer works" << std::endl;
}

void testFlatRenderer() {
    Color color(1.0, 2.0, 3.0);
    auto material = std::make_shared<DiffuseMaterial>(std::make_shared<UniformTexture>(color));
    Sphere sphere(material, translation(Vec3(2., 0., 0.)) * scaling(Vec3(0.2, 0.2, 0.2)));
    Camera camera("orthogonal", 1., 3);
    World world;
    world.addShape(std::make_shared<Sphere>(sphere));
    camera.render(Renderers::Flat, world);

    sassert(camera.image.getPixel(0, 0).isClose(BLACK));
    sassert(camera.image.getPixel(1, 0).isClose(BLACK));
    sassert(camera.image.getPixel(2, 0).isClose(BLACK));

    sassert(camera.image.getPixel(0, 1).isClose(BLACK));
    sassert(camera.image.getPixel(1, 1).isClose(color)); // account for the factor 1/PI in the reflectance
    sassert(camera.image.getPixel(2, 1).isClose(BLACK));

    sassert(camera.image.getPixel(0, 2).isClose(BLACK));
    sassert(camera.image.getPixel(1, 2).isClose(BLACK));
    sassert(camera.image.getPixel(2, 2).isClose(BLACK));

    std::cout << "flat renderer works" << std::endl;
}

void testPathTracer() { // furnace test
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

        sassert(areClose(result.r, expected, 1e-3));
        sassert(areClose(result.g, expected, 1e-3));
        sassert(areClose(result.b, expected, 1e-3));
    }

    std::cout << "furnace tests works" << std::endl;
}



int main() {
    testOnOffRenderer();
    testFlatRenderer();
    testPathTracer();

    std::cout << "All tests passed!\n";

    return 0;
}