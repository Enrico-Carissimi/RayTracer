#include "../Camera.hpp"
#include "../renderers.hpp"
#include "../utils.hpp"

const Color BLACK(0., 0., 0.);
const Color WHITE(1., 1., 1.);

void testOnOffRenderer() {
    auto material = std::make_shared<Diffuse>(std::make_shared<Uniform>(WHITE));
    Sphere sphere(material, translation(Vec3(2., 0., 0.)) * scaling(Vec3(0.2, 0.2, 0.2)));

    Camera camera("orthogonal", 1., 3.);
    World world;
    world.addShape(std::make_shared<Sphere>(sphere));
    camera.render(world, Renderers::OnOff);

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
    auto material = std::make_shared<Diffuse>(std::make_shared<Uniform>(color));
    Sphere sphere(material, translation(Vec3(2., 0., 0.)) * scaling(Vec3(0.2, 0.2, 0.2)));
    Camera camera("orthogonal", 1., 3);
    World world;
    world.addShape(std::make_shared<Sphere>(sphere));
    camera.render(world, Renderers::Flat);

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



int main() {
    testOnOffRenderer();
    testFlatRenderer();

    return 0;
}