#include "../Camera.hpp"
#include "../renderers.hpp"
#include "../utils.hpp"

const Color BLACK(0.0f, 0.0f, 0.0f);
const Color WHITE(1.0f, 1.0f, 1.0f);
const Color RED(1.0f, 0.0f, 0.0f);
const Color GREEN(0.0f, 1.0f, 0.0f);
const Color BLUE(0.0f, 0.0f, 1.0f);
const Color YELLOW(1.0f, 1.0f, 0.0f);

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
    sassert(camera.image.getPixel(1, 1).isClose(color * (1. / PI))); // account for the factor 1/PI in the reflectance
    sassert(camera.image.getPixel(2, 1).isClose(BLACK));

    sassert(camera.image.getPixel(0, 2).isClose(BLACK));
    sassert(camera.image.getPixel(1, 2).isClose(BLACK));
    sassert(camera.image.getPixel(2, 2).isClose(BLACK));

    std::cout << "flat renderer works" << std::endl;
}

void testPointLight() {
    World world;

    auto sphereMaterial = std::make_shared<DiffuseMaterial>(
        std::make_shared<UniformTexture>(RED),
        std::make_shared<UniformTexture>(BLUE)
    );

    world.addShape(std::make_shared<Sphere>(sphereMaterial));

    Point3 lightPos(-2.0f, -1.0f, 0.0f);

    world.addLight(PointLight{
        lightPos,
        YELLOW
    });

    Ray ray(Point3(-2.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f));

    Color color = Renderers::PointLight(ray, world, GREEN);

    float cosTheta = std::cos(PI / 4.0f);  // θ = 45°
    Color brdf = RED * (1.0f / PI);
    Color expected = GREEN + (BLUE + brdf) * YELLOW * cosTheta;

    if (!areClose(expected.r, color.r) || !areClose(expected.g, color.g) || !areClose(expected.b, color.b)) {
        std::cerr << "Test failed!\n";
        std::cerr << "Expected: (" << expected.r << ", " << expected.g << ", " << expected.b << ")\n";
        std::cerr << "Got:      (" << color.r << ", " << color.g << ", " << color.b << ")\n";
    } else {
        std::cout << "Test passed!\n";
    }
}



int main() {
    testOnOffRenderer();
    testFlatRenderer();
    testPointLight();

    return 0;
}