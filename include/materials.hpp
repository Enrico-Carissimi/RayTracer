#ifndef __material__
#define __material__

#include "Color.hpp"
#include "Vec2.hpp"
#include "HDRImage.hpp"
#include "HitRecord.hpp"
#include <memory>

/**
 * @class Texture
 * @brief Abstract base class for texture mapping.
 * 
 * Provides an interface for obtaining color values at given UV coordinates.
 */
class Texture {
public:
    virtual ~Texture() = default;
    virtual Color color(const Vec2& uv) const = 0;
};

/**
 * @class UniformTexture
 * @brief Texture that returns a uniform (constant) color.
 */
class UniformTexture : public Texture {
public:
    UniformTexture() : _color(Color(0., 0., 0.)) {}
    UniformTexture(const Color& c) : _color(c) {}

    Color color(const Vec2& uv) const override {
        return _color;
    }

private:
    Color _color;
};

/**
 * @class CheckeredTexture
 * @brief Texture that generates a 2D checkered pattern.
 */
class CheckeredTexture : public Texture {
public:
    CheckeredTexture() : _color1(Color(0., 0., 0.)), _color2(Color(1., 1., 1.)), _nSteps(8) {}
    CheckeredTexture(const Color& c1, const Color& c2, int nSteps = 8) : _color1(c1), _color2(c2), _nSteps(nSteps) {}

    Color color(const Vec2& uv) const override {
        int u = std::floor(uv.u * _nSteps);
        int v = std::floor(uv.v * _nSteps);
        return (u + v) % 2 == 0 ? _color1 : _color2;
    }

private:
    int _nSteps;
    Color _color1, _color2;
};

/**
 * @class ImageTexture
 * @brief Texture based on an HDR image.
 */
class ImageTexture : public Texture {
public:
    ImageTexture(const std::string& name) : _PFM(name) {_PFM.normalize(1.); _PFM.clamp();}
    ImageTexture(const HDRImage& image) : _PFM(image) {}

    Color color(const Vec2& uv) const override {
        int i = uv.u * _PFM._width;
        int j = uv.v * _PFM._height;

        i -= (i >= _PFM._width), j -= (j >= _PFM._height); // if i >= width then i = i - 1

        return _PFM.getPixel(i, j); // interpolation!
    }

private:
    HDRImage _PFM;
};


/**
 * @class Material
 * @brief Abstract base class representing a material.
 * 
 * Contains texture and emitted radiance, and defines interfaces
 * for evaluation and scattering of rays.
 */
class Material {
public:
    Material() : _texture(std::make_shared<UniformTexture>(Color(0., 0., 0.))), _emittedRadiance(std::make_shared<UniformTexture>(Color(0., 0., 0.))) {};
    Material(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> emittedRadiance = std::make_shared<UniformTexture>(Color()))
        : _texture(texture), _emittedRadiance(emittedRadiance) {}
    virtual ~Material() = default;
    
    Color color(const Vec2& uv) { return _texture->color(uv); }
    Color emittedColor(const Vec2& uv) { return _emittedRadiance->color(uv); }
    
    virtual Color eval(const Vec2& uv, float thetaIn, float thetaOut) const = 0;
    virtual Ray scatterRay(PCG& pcg, const Vec3& incomingDir, const Point3& interactionPoint,
                           const Normal3& normal, int depth) const = 0;

protected:
    std::shared_ptr<Texture> _texture;
    std::shared_ptr<Texture> _emittedRadiance;
};

/**
 * @class DiffuseMaterial
 * @brief Lambertian diffuse material with cosine-weighted scattering.
 */
class DiffuseMaterial : public Material {
public:
    DiffuseMaterial() : Material(), _reflectance(1.) {}
    DiffuseMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> emittedRadiance = std::make_shared<UniformTexture>(Color()), float reflectance = 1.)
        : Material(texture, emittedRadiance), _reflectance(reflectance / PI) {}

    Color eval(const Vec2& uv, float thetaIn = 0., float thetaOut = 0.) const override {
        return _texture->color(uv) * _reflectance;
    }

    Ray scatterRay(PCG& pcg, const Vec3& incomingDir, const Point3& interactionPoint, const Normal3& normal, int depth) const override {
        // cosine-weighted distribution around the z (local) axis
        Vec3 n(normal.x, normal.y, normal.z);
        Vec3 e1, e2;
        createONB(n, e1, e2);
        float cos2 = pcg.random();
        float cosTheta = std::sqrt(cos2), sinTheta = std::sqrt(1. - cos2);
        float phi = 2. * PI * pcg.random();

        return Ray(interactionPoint, e1 * std::cos(phi) * cosTheta + e2 * std::sin(phi) * cosTheta + n * sinTheta, 1e-5, INF, depth);
    }

private:
    float _reflectance; // is already divided by pi in the constructor
};

/**
 * @class SpecularMaterial
 * @brief Perfect mirror-like reflective material.
 */
class SpecularMaterial : public Material {
public:
    SpecularMaterial() : Material(), _thresholdAngleRad(PI / 1800.0) {}
    SpecularMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> emittedRadiance = std::make_shared<UniformTexture>(Color()), float thresholdAngleRad = PI / 1800.0)
        : Material(texture, emittedRadiance), _thresholdAngleRad(thresholdAngleRad) {}

    Color eval(const Vec2& uv, float thetaIn, float thetaOut) const override {
        if (std::abs(thetaIn - thetaOut) < _thresholdAngleRad) {
            return _texture->color(uv);
        } else {
            return Color(0.0, 0.0, 0.0);
        }
    };

    Ray scatterRay(PCG& pcg, const Vec3& incomingDir, const Point3& interactionPoint,
                   const Normal3& normal, int depth) const override {
        Vec3 rayDir = incomingDir.normalize();
        Vec3 norm(normal.x, normal.y, normal.z);
        norm = norm.normalize();
        double dotProd = dot(norm, rayDir);

        Vec3 reflectedDir = rayDir - norm * 2.0 * dotProd;

        return Ray{interactionPoint, reflectedDir, 1e-5, INF, depth};
    }

private:
    double _thresholdAngleRad;
};

#endif