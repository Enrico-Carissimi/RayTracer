#ifndef __HDRImage__
#define __HDRImage__

#include <iostream>
#include <vector>
#include "Color.hpp"

class HDRImage {
public:
    HDRImage(int width, int height) : _width(width), _height(height) {
        _pixels = std::vector<Color>(_width * _height);

        for (int i = 0; i < _width * _height; i++) {
            _pixels[i] = Color(); // defaults to black
        }
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

private:
    int _width, _height;
    std::vector<Color> _pixels; // maybe use smart pointers?
};

#endif