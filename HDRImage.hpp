#ifndef __HDRImage__
#define __HDRImage__

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "Color.hpp"
#include "PFMreader.hpp"

float clamp(float x) {

    return x / (1 + x);
}

class HDRImage {
public:
    HDRImage(int width, int height) : _width(width), _height(height) {
        _pixels = std::vector<Color>(_width * _height);

        for (int i = 0; i < _width * _height; i++) {
            _pixels[i] = Color(); // defaults to black
        }
    }

    HDRImage(std::istream& input) {
        readPFM(input);
    }

    HDRImage(const std::string& fileName) {
        std::ifstream input(fileName);
        readPFM(input);
    }

    int pixelIndex(int i, int j) {
        return i + _width * j;
    }

    bool validCoordinates(int i, int j) {
        return (i >= 0 && i < _width && j >= 0 && j < _height);
    }

    void checkCoordinates(int i, int j) {
        if (!validCoordinates(i, j)) {
            std::cout << "ERROR: trying to access invalid image coordinates (" << i << ", " << j << ")\n";
            std::cout << "image size is " << _width << " x " << _height << std::endl;
            exit(-1);
        }
        return;
    }

    Color getPixel(int i, int j) {
        checkCoordinates(i, j);
        return _pixels[pixelIndex(i, j)];
    }

    void setPixel(int i, int j, Color color) {
        checkCoordinates(i, j);
        _pixels[pixelIndex(i, j)] = color;
        return;
    }

    float averageLuminosity(float delta = 1e-10){

        float sum = 0.0;
        for (const Color &pixel : _pixels) {
            sum += std::log10(pixel.luminosity() + delta);
        }

        sum /= _pixels.size();

        return std::pow(10, sum);

    }

    float normalizeImage(float a, float luminosity = 0.0) {

        if (luminosity == 0.0) {

            luminosity = averageLuminosity();
            return luminosity;
        }

        //calculate the scale factor
        float scale = a / luminosity;

        for (Color& pixel: _pixels) {

            pixel = pixel * scale;
        }
    }

    void clampImage() {
        for (Color& pixel: _pixels) {
            pixel.r = clamp(pixel.r);
            pixel.g = clamp(pixel.g);
            pixel.b = clamp(pixel.b);
        }
    }

    int _width, _height;

private:
    std::vector<Color> _pixels; // maybe use smart pointers?

    void readPFM(std::istream& input) {
        // magic
        std::string magic = readLine(input);
        if (magic != "PF") {
            throw std::invalid_argument("ERROR: invalid magic string \"" + magic + "\", must be \"PF\" for a PFM file");
        }
        
        // image dimensions (width, height)
        auto [width, height] = parseImageSize(readLine(input)); // maybe [_width, _height] = ... works?
        _width = width, _height = height;
    
        // endianness
        auto endianness = parseEndianness(readLine(input));
    
        
        // fill the image
        _pixels = std::vector<Color>(_width * _height);
        for (int j = height - 1; j >= 0; j--) {
            for (int i = 0; i < width; i++) {
                float r = readFloat(input, endianness), g = readFloat(input, endianness), b = readFloat(input, endianness);
                setPixel(i, j, Color(r, g, b));
            }
        }
    }
};

#endif