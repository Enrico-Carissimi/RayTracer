#include "World.hpp"

namespace Renderers {

auto OnOff = [](Ray ray, const World& world) {
    HitRecord rec;
    return world.isHit(ray, rec) ? Color(1., 1., 1.) : Color(0., 0., 0.);
};

auto Flat = [](Ray ray, const World& world) {
    HitRecord rec;
    return world.isHit(ray, rec) ? rec.material->eval(rec.surfacePoint) : Color(0., 0., 0.);
};

}