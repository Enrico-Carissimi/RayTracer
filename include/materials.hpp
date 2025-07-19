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
    UniformTexture() : _color(Color()) {}
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
    CheckeredTexture() : _color1(Color(0.0f, 0.0f, 0.0f)), _color2(Color(1.0f, 1.0f, 1.0f)), _nSteps(8) {}
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
    ImageTexture(const std::string& name) : _PFM(name) {_PFM.normalize(1.0f); _PFM.clamp();}
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
    Material() : _texture(std::make_shared<UniformTexture>(Color())), _emittedRadiance(std::make_shared<UniformTexture>(Color())) {};
    Material(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> emittedRadiance = std::make_shared<UniformTexture>(Color()))
        : _texture(texture), _emittedRadiance(emittedRadiance) {}
    virtual ~Material() = default;
    
    Color color(const Vec2& uv) { return _texture->color(uv); }
    Color emittedColor(const Vec2& uv) { return _emittedRadiance->color(uv); }
    
    virtual Color eval(const Vec2& uv, float thetaIn, float thetaOut) const = 0;
    virtual Ray scatterRay(PCG& pcg, const HitRecord& rec, int depth) const = 0;

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
    DiffuseMaterial() : Material(), _reflectance(1.0f) {}
    DiffuseMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> emittedRadiance = std::make_shared<UniformTexture>(Color()), float reflectance = 1.0f)
        : Material(texture, emittedRadiance), _reflectance(reflectance / PI) {}

    Color eval(const Vec2& uv, float thetaIn = 0.0f, float thetaOut = 0.0f) const override {
        return _texture->color(uv) * _reflectance;
    }

    Ray scatterRay(PCG& pcg, const HitRecord& rec, int depth) const override {
        return Ray(rec.worldPoint, pcg.sampleHemisphere(rec.normal), 1e-5f, INF, depth);
    }

private:
    float _reflectance; // is already divided by pi in the constructor
};

/**
 * @class SpecularMaterial
 * @brief Mirror-like reflective material, with adjustable "blurriness".
 */
class SpecularMaterial : public Material {
public:
    SpecularMaterial() : Material(), _blur(0.0f), _thresholdAngleRad(PI / 1800.0f) {}
    SpecularMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> emittedRadiance = std::make_shared<UniformTexture>(Color()),
                     float blur = 0.0f, float thresholdAngleRad = PI / 1800.0f)
        : Material(texture, emittedRadiance), _blur(blur), _thresholdAngleRad(thresholdAngleRad) {}

    Color eval(const Vec2& uv, float thetaIn, float thetaOut) const override {
        if (std::abs(thetaIn - thetaOut) < _thresholdAngleRad) {
            return _texture->color(uv);
        } else {
            return Color(0.0f, 0.0f, 0.0f);
        }
    };

    Ray scatterRay(PCG& pcg, const HitRecord& rec, int depth) const override {
        Vec3 reflectedDir = reflect(rec.ray.direction.normalize(), rec.normal.normalize());

        // Moves the reflected direction by a random vector of length _blur
        // in the same direction of the reflection. I don't think this is physical,
        // but is probably better than sampling the cone uniformly.
        // !!! The ray could go back inside the object...
        if (_blur > 0.0f) {
            reflectedDir += _blur * pcg.sampleHemisphere(reflectedDir);
        }

        return Ray{rec.worldPoint, reflectedDir, 1e-5f, INF, depth};
    }

private:
    float _blur, _thresholdAngleRad;
};

/**
 * @class TransparentMaterial
 * @brief Transparent material that refracts light.
 */
class TransparentMaterial : public Material {
public:
    TransparentMaterial() : Material(), _refractionIndex(1.0f) {}
    TransparentMaterial(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> emittedRadiance = std::make_shared<UniformTexture>(Color()), float refractionIndex = 1.0f)
        : Material(texture, emittedRadiance), _refractionIndex(refractionIndex), _inverseRefractionIndex(1.0f / refractionIndex) {}

    Color eval(const Vec2& uv, float thetaIn = 0.0f, float thetaOut = 0.0f) const override { // to be changed
        return _texture->color(uv) * (1.0f / PI); // divide by pi to be consistent with the diffuse material (for now)
    }

    Ray scatterRay(PCG& pcg, const HitRecord& rec, int depth) const override {
        // _refractioIndex is actually n1/n2, n1 is the RI of this object, n2 of the outside.
        // n1/n2 sin(theta1) = sin(theta2), where 1 is the material where the ray starts, and 2 where it enters.
        // So we use _refractionIndex when going from inside this object to outside,
        // and _inverseRefractionIndex when the rays enters this object.
        float RI = rec.isInside ? _refractionIndex : _inverseRefractionIndex;
        
        Vec3 refractedDir = refract(rec.ray.direction.normalize(), rec.normal.normalize(), RI);
        return Ray(rec.worldPoint, refractedDir, 1e-5f, INF, depth);
    }

private:
    float _refractionIndex, _inverseRefractionIndex;
};

#endif