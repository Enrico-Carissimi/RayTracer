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

class UniformTexture : public Texture {
public:
    UniformTexture() : _color(Color(0., 0., 0.)) {}
    UniformTexture(const Color& c) : _color(c) {}

    Color color(Vec2 uv) const override {
        return _color;
    }

private:
    Color _color;
};

class CheckeredTexture : public Texture {
public:
    CheckeredTexture() : _color1(Color(0., 0., 0.)), _color2(Color(1., 1., 1.)), _nSteps(8) {}
    CheckeredTexture(const Color& c1, const Color& c2, int nSteps = 8) : _color1(c1), _color2(c2), _nSteps(nSteps) {}

    Color color(Vec2 uv) const override {
        int u = std::floor(uv.u * _nSteps);
        int v = std::floor(uv.v * _nSteps);
        return (u + v) % 2 == 0 ? _color1 : _color2;
    }

private:
    int _nSteps;
    Color _color1, _color2;
};

class ImageTexture : public Texture {
public:
    ImageTexture(const std::string& name) : _PFM(name) {_PFM.normalize(1.); _PFM.clamp();}
    ImageTexture(const HDRImage image) : _PFM(image) {}

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
    Material() : _texture(std::make_shared<UniformTexture>(Color(0., 0., 0.))) {};
    Material(std::shared_ptr<Texture> texture) : _texture(texture) {}
    virtual ~Material() = default;
    virtual Color eval(Vec2 uv) const = 0;
    virtual Ray scatterRay(const PCG& pcg, const Vec3& incomingDir, const Point3& interactionPoint,
                           const Normal3& normal, int depth) const = 0;
    virtual Color emittedRadiance(Vec2 uv) const {
        return Color(0.0f, 0.0f, 0.0f);
}


protected:
    std::shared_ptr<Texture> _texture;
};

class DiffuseMaterial : public Material {
public:
    DiffuseMaterial() : Material(), _reflectance(1.) {}
    DiffuseMaterial(std::shared_ptr<Texture> texture, float reflectance = 1.) : Material(texture), _reflectance(reflectance / PI) {}

    Color eval(Vec2 uv) const override {
        return _texture->color(uv) * _reflectance;
    }

private:
    float _reflectance; // is already divided by pi in the constructor
};

class SpecularMaterial : public Material {
public:
    SpecularMaterial(std::shared_ptr<Texture> texture = std::make_shared<UniformTexture>(Color(1.0f, 1.0f, 1.0f)),
                 double thresholdAngleRad = PI / 1800.0)
        : Material(texture), thresholdAngleRad(thresholdAngleRad) {}

        Color eval(double thetaIn, double thetaOut, const Vec2& uv) const {
        if (std::abs(thetaIn - thetaOut) < thresholdAngleRad) {
            return _texture->color(uv);
        } else {
            return Color(0.0, 0.0, 0.0);
        }
    };

    Ray scatterRay(const PCG& pcg, const Vec3& incomingDir, const Point3& interactionPoint,
                    const Normal3& normal, int depth) const override {
        Vec3 rayDir = incomingDir.normalize();
        Vec3 norm(normal.x, normal.y, normal.z);
        norm = norm.normalize();
        double dotProd = dot(norm, rayDir);

        Vec3 reflectedDir = rayDir - norm * 2.0 * dotProd;

        return Ray{interactionPoint, reflectedDir, 1e-5, INF, depth};
    }

private:
    double thresholdAngleRad;
};

#endif