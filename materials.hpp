#ifndef __material__
#define __material__

#include "Color.hpp"
#include "Vec2.hpp"
#include "HDRImage.hpp"
#include "HitRecord.hpp"
#include <memory>

class Texture { // I don't like the name "pigment"
public:
    virtual ~Texture() = default;
    virtual Color color(Vec2 uv) const = 0;
};

class Uniform : public Texture {
public:
    Uniform() : _color(Color(0., 0., 0.)) {}
    Uniform(const Color& c) : _color(c) {}

    Color color(Vec2 uv) const override {
        return _color;
    }

private:
    Color _color;
};

class Checkered : public Texture {
public:
    Checkered() : _color1(Color(0., 0., 0.)), _color2(Color(1., 1., 1.)), _nSteps(8) {}
    Checkered(const Color& c1, const Color& c2, int nSteps = 8) : _color1(c1), _color2(c2), _nSteps(nSteps) {}

    Color color(Vec2 uv) const override {
        int u = std::floor(uv.u * _nSteps);
        int v = std::floor(uv.v * _nSteps);
        return (u + v) % 2 == 0 ? _color1 : _color2;
    }

private:
    int _nSteps;
    Color _color1, _color2;
};

class FromImage : public Texture {
public:
    FromImage(const std::string& name) : _PFM(name) {_PFM.normalize(1.); _PFM.clamp();}
    FromImage(const HDRImage image) : _PFM(image) {}

    Color color(Vec2 uv) const override {
        int i = uv.u * _PFM._width;
        int j = uv.v * _PFM._height;

        i -= (i >= _PFM._width), j -= (j >= _PFM._width); // if i >= width then i = i - 1

        return _PFM.getPixel(i, j); // interpolation!
    }

private:
    HDRImage _PFM;
};



class Material {
public:
    Material() : _texture(std::make_shared<Uniform>(Color(0., 0., 0.))) {};
    Material(std::shared_ptr<Texture> texture) : _texture(texture) {}
    virtual ~Material() = default;
    virtual Color eval(Vec2 uv) const = 0;

protected:
    std::shared_ptr<Texture> _texture;
};

class Diffuse : public Material {
public:
    Diffuse() : Material(), _reflectance(1.) {}
    Diffuse(std::shared_ptr<Texture> texture, float reflectance = 1.) : Material(texture), _reflectance(reflectance / PI) {}

    Color eval(Vec2 uv) const override {
        return _texture->color(uv) * _reflectance;
    }

private:
    float _reflectance; // is already divided by pi in the constructor
};

#endif