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

/**
 * @brief Casts a ray using an orthogonal projections.
 * 
 * @param u horizontal coordinate of the image
 * @param v vertical coordinate of the image
 * @param d distance of the observer from the image, unused
 * @param a aspect ratio of the image
 * @return Ray 
 */
inline Ray _castOrthogonal(float u, float v, float d, float a) {
    Point3 origin = Point3(-d, (1.0f - 2.0f * u) * a, 2.0f * v - 1.0f);
    Vec3 direction(1.0f, 0.0f, 0.0f); // along x axis
    return Ray(origin, direction);
}

/**
 * @brief Casts a ray using a perspective projections.
 * 
 * @param u horizontal coordinate of the image
 * @param v vertical coordinate of the image
 * @param d distance of the observer from the image, defines the FOV
 * @param a aspect ratio of the image
 * @return Ray 
 */
inline Ray _castPerspective(float u, float v, float d, float a) {
    Point3 origin(-d, 0.0f, 0.0f);
    Vec3 direction(d, (1.0f - 2.0f * u) * a, 2.0f * v - 1.0f);
    return Ray(origin, direction);
}



/**
 * @class Camera
 * @brief Camera used to observe (and render) a scene.
 * 
 */
class Camera {
public:
    int imageWidth, imageHeight;
    float aspectRatio;
    Transformation transformation;
    HDRImage image;
    PCG pcg;

    /**
     * @brief Construct a new Camera object.
     * 
     * @param type "orthogonal" or "perspective", to use orthogonal or perspective camera projections
     * @param aspectRatio 
     * @param imageWidth in pixels
     * @param distance the distance between the camera and the image (perspective only)
     * @param transformation used to move the camera around the scene
     * @param pcg the random number generator
     */
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

    /**
     * @brief Casts a ray towards the pixel (i, j) of the image, at coordinates (u, v) on the screen.
     * 
     * u and v range from 0 to 1 and are then mapped in [-1, 1]. The top-left corner is (1, 0), the bottom right is (0, 1).
     * (uPixel, vPixel) is the position inside the pixel, (0.5, 0.5) is the centre.
     * 
     * @param i horizontal coordinate of the pixel
     * @param j vertical coordinate of the pixel
     * @param uPixel horizontal coordinate inside the pixel
     * @param vPixel vertical coordinate inside the pixel
     * @return Ray 
     */
    inline Ray castRay(int i, int j, float uPixel = 0.5f, float vPixel = 0.5f) const {
        float u = ((float)i + uPixel) / imageWidth;
        float v = 1.0f - ((float)j + vPixel) / imageHeight;
        return _castRay(u, v, _distance, aspectRatio).transform(transformation); // maybe not the best way to do this
    }

    /**
     * @brief Casts rays to every pixel of the image and computes their color using a renderer.
     * 
     * @tparam Renderer 
     * @tparam Args 
     * @param renderer algorithm used to render the image
     * @param AASamples number of samples per pixel used for anti-aliasing
     * @param args additional arguments needed by "renderer", the first is always the World to render
     */
    template <typename Function, typename... Args>
    void render(const Function& renderer, int AASamples, Args&&... args) { // first arg should be the world
        int AASamplesRoot = std::round(std::sqrt(AASamples));
        bool squareAA = (AASamplesRoot * AASamplesRoot == AASamples);

        auto start = std::chrono::steady_clock::now();

        for (int j = 0; j < imageHeight; j++) {
            std::cout << "\rdrawing row " << j + 1 << "/" << imageHeight << std::flush;

            for (int i = 0; i < imageWidth; i++) {
                // After some tests, moving the ifs outside the loops
                // result in a negligible (or even absent) improvement in speed.
                // The code is more readable and simpler this way

                // no antialiasing
                if (AASamples == 1) {
                    image.setPixel(i, j, samplePixel(i, j, 0.5f, 0.5f, renderer, std::forward<Args>(args)...));
                    continue;
                }

                // antialiasing
                if (!squareAA) { // non-square number of samples
                    antialiasing(i, j, AASamples, renderer, std::forward<Args>(args)...);
                } else {         // square number of samples
                    stratifiedSampling(i, j, AASamplesRoot, renderer, std::forward<Args>(args)...);
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

    /**
     * @brief Samples the color of a pixel, using the chosen rendering algorithm.
     * 
     * @tparam Function 
     * @tparam Args 
     * @param i pixel horizontal coordinate
     * @param j pixel vertical coordinate
     * @param uPixel horizontal coordinate inside the pixel
     * @param vPixel vertical coordinate inside the pixel
     * @param renderer algorithm used to render the image
     * @param args additional arguments needed by "renderer", the first is always the World to render
     * @return Color 
     */
    template <typename Function, typename... Args>
    Color samplePixel(int i, int j, float uPixel, float vPixel, const Function& renderer, Args&&... args) {
        Ray ray = castRay(i, j, uPixel, vPixel);
        return renderer(ray, std::forward<Args>(args)...);
    }

    /**
     * @brief Casts rays at random inside pixel (i, j) and averages the color.
     * 
     * @tparam Function 
     * @tparam Args 
     * @param i pixel horizontal coordinate
     * @param j pixel vertical coordinate
     * @param AASamples number of random samples used to compute the color of the pixel
     * @param renderer algorithm used to render the image
     * @param args additional arguments needed by "renderer", the first is always the World to render
     */
    template <typename Function, typename... Args>
    void antialiasing(int i, int j, int AASamples, const Function& renderer, Args&&... args) {
        Color sum;

        for (int aa = 0; aa < AASamples; aa++) {
            sum += samplePixel(i, j, pcg.random(), pcg.random(), renderer, std::forward<Args>(args)...);
        }

        image.setPixel(i, j, sum * (1.0f / AASamples));
    }

    /**
     * @brief Divides pixel (i, j) in a square grid, casts a ray randomly in each cell, then averages the color.
     * 
     * @tparam Function 
     * @tparam Args 
     * @param i pixel horizontal coordinate
     * @param j pixel vertical coordinate
     * @param side side of the square grid used to sample the pixel
     * @param renderer algorithm used to render the image
     * @param args additional arguments needed by "renderer", the first is always the World to render
     */
    template <typename Function, typename... Args>
    void stratifiedSampling(int i, int j, int side, const Function& renderer, Args&&... args) {
        Color sum;
        
        for (int jPixel = 0; jPixel < side; jPixel++) {
            for (int iPixel = 0; iPixel < side; iPixel++) {
                float uPixel = (iPixel + pcg.random()) / side, vPixel = (jPixel + pcg.random()) / side;

                sum += samplePixel(i, j, uPixel, vPixel, renderer, std::forward<Args>(args)...);
            }
        }

        image.setPixel(i, j, sum * (1.0f / (side * side)));
    }
};

#endif