#ifndef __Camera__
#define __Camera__

#include <utility>

#include "Point3.hpp"
#include "Vec3.hpp"
#include "Transformation.hpp"
#include "HDRImage.hpp"
#include "Ray.hpp"
#include "World.hpp"



using _CastRay = Ray(float u, float v, float d, float a);

Ray _castOrthogonal(float u, float v, float d, float a) {
    Point3 origin = Point3(-d, (1. - 2. * u) * a, 2. * v - 1.);
    Vec3 direction(1., 0., 0.); // along x axis
    return Ray(origin, direction);
}

Ray _castPerspective(float u, float v, float d, float a) {
    Point3 origin(-d, 0., 0.);
    Vec3 direction(d, (1. - 2. * u) * a, 2. * v - 1.);
    return Ray(origin, direction);
}



class Camera {
public:
    int imageWidth, imageHeight;
    float aspectRatio;
    Transformation transformation;
    HDRImage image;
    PCG pcg;

    // use type = "orthogonal" or "perspective" to use orthogonal or perspective camera projections
    // distance is the distance between the camera and the image (perspective only)
    // move the camera with a transfomation
    Camera(std::string type, float aspectRatio, int imageWidth, float distance = 1.0f, Transformation transformation = Transformation(), PCG pcg = PCG()) :
    aspectRatio(aspectRatio), imageWidth(imageWidth), imageHeight(imageWidth / aspectRatio),
    transformation(transformation), image(imageWidth, imageHeight), pcg(pcg) {
        if (type == "orthogonal"){
            _castRay = _castOrthogonal;
            _distance = 1.0f;
            return;
        }
        if (type == "perspective"){
            _castRay = _castPerspective;
            _distance = distance;
            return;
        }
        std::cout << "ERROR: invalid camera type, use \"orthogonal\" or \"perspective\"";
        exit(-1);
    }

    // casts a ray towards the pixel (i, j) of the image, at coordinates (u, v) on the screen, with u and v ranging from 0 to 1
    // u and v are then mapped in [-1, 1]
    // (uPixel, vPixel) is the position inside the pixel, (0.5, 0.5) is the centre
    // the top-left corner is (1, 0), bottom right is (0, 1)
    inline Ray castRay(int i, int j, float uPixel = 0.5, float vPixel = 0.5) const {
        float u = ((float)i + uPixel) / imageWidth;
        float v = 1 - ((float)j + vPixel) / imageHeight;
        return _castRay(u, v, _distance, aspectRatio).transform(transformation); // maybe not the best way to do this
    }

    // casts rays to every pixel of the image and computes their color using a renderer
    template <typename Function, typename... Args>
    void render(const Function& renderer, int AASamples, Args&&... args) { // first arg should be the world
        int AASamplesRoot = std::round(std::sqrt(AASamples));
        bool squareAA = (AASamplesRoot * AASamplesRoot == AASamples);

        auto start = std::chrono::steady_clock::now();

        for (int j = 0; j < imageHeight; j++) {
            std::cout << "\rdrawing row " << j + 1 << "/" << imageHeight << std::flush;

            for (int i = 0; i < imageWidth; i++) {

                // no antialiasing
                if (AASamples == 1) {
                    image.setPixel(i, j, samplePixel(i, j, 0.5, 0.5, renderer, args...));
                    continue;
                }

                // antialiasing
                if (!squareAA) {                                         // non-square number of samples
                    antialiasing(i, j, AASamples, renderer, args...);
                } else {                                                 // square number of samples
                    stratifiedSampling(i, j, AASamplesRoot, renderer, args...);
                }
            }
        }

        auto end = std::chrono::steady_clock::now();

        std::cout << "\rimage drawn in " << std::fixed << std::setprecision(2)
                  << std::chrono::duration<float>(end - start).count() << " s                 " << std::endl;
    }

private:
    float _distance;
    _CastRay* _castRay;

    template <typename Function, typename... Args>
    Color samplePixel(int i, int j, float uPixel, float vPixel, const Function& renderer, Args&&... args) {
        Ray ray = castRay(i, j, uPixel, vPixel);
        return renderer(ray, std::forward<Args>(args)...); // c++ magic
    }

    template <typename Function, typename... Args>
    void antialiasing(int i, int j, int AASamples, const Function& renderer, Args&&... args) {
        Color sum;

        for (int aa = 0; aa < AASamples; aa++) {
            sum += samplePixel(i, j, pcg.random(), pcg.random(), renderer, std::forward<Args>(args)...);
        }

        image.setPixel(i, j, sum * (1. / AASamples));
    }

    template <typename Function, typename... Args>
    void stratifiedSampling(int i, int j, int side, const Function& renderer, Args&&... args) {
        Color sum;
        
        for (int jPixel = 0; jPixel < side; jPixel++) {
            for (int iPixel = 0; iPixel < side; iPixel++) {
                float uPixel = (iPixel + pcg.random()) / side, vPixel = (jPixel + pcg.random()) / side;

                sum += samplePixel(i, j, uPixel, vPixel, renderer, std::forward<Args>(args)...);
            }
        }

        image.setPixel(i, j, sum * (1. / (side * side)));
    }
};

#endif