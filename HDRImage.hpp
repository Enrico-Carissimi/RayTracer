#ifndef __HDRImage__
#define __HDRImage__

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <filesystem>
#include <string>

#include "Color.hpp"
#include "PFMreader.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION // needed for stb
#include "lib/stb_image_write.h"



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
        std::ifstream input(fileName, std::ios::binary);
        if (input.fail()) throw std::runtime_error("ERROR: impossible to open file \"" + fileName + "\"");
        readPFM(input);
        input.close();
    }

    int pixelIndex(int i, int j) {
        return i + _width * j;
    }

    bool validCoordinates(int i, int j) {
        return (i >= 0 && i < _width && j >= 0 && j < _height);
    }

    void checkCoordinates(int i, int j) {
        if (!validCoordinates(i, j)) {
            throw std::invalid_argument("ERROR: trying to access invalid image coordinates (" + std::to_string(i) + ", " + std::to_string(j) + "), "
                                        + "image size is " + std::to_string(_width) + " x " + std::to_string(_height));
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
        for (const Color& pixel : _pixels) {
            sum += std::log10(pixel.luminosity() + delta);
        }

        sum /= _pixels.size();

        return std::pow(10, sum);
    }

    void normalize(float a, float luminosity = 0.0) {
        if (luminosity == 0.0) {
            luminosity = averageLuminosity();
        }

        // calculate the scale factor
        float scale = a / luminosity;

        for (Color& pixel: _pixels) {
            pixel = pixel * scale;
        }
    }

    void clamp() {
        for (Color& pixel: _pixels) {
            pixel.r = ::clamp(pixel.r);
            pixel.g = ::clamp(pixel.g);
            pixel.b = ::clamp(pixel.b);
        }
    }

    // @brief saves the image as the format specified by the extension of fileName (pfm, png, jpeg)
    void save(std::string fileName, float gamma) {
        auto extension = std::filesystem::path(fileName).extension();
        if (extension == ".pfm") {writePFM(fileName); return;}
        if (extension == ".png") {writePNG(fileName, gamma); return;}
        if (extension == ".jpg" || extension == ".jpeg") {writeJPG(fileName, gamma); return;}
        throw std::invalid_argument("ERROR: file extension \"" + extension.string() + "\" is not supported");
    }

    int _width, _height;

private:
    std::vector<Color> _pixels;

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
        for (int j = _height - 1; j >= 0; j--) {
            for (int i = 0; i < _width; i++) {
                float r = readFloat(input, endianness), g = readFloat(input, endianness), b = readFloat(input, endianness);
                setPixel(i, j, Color(r, g, b)); // readFloat can't be called inside the constructor because r, g, b are not evaluated in order
            }
        }
    }

    //@brief converts the pixels to 1 byte ints from 0 to 255;
    //@brief the HDR pixels must be normalized before calling this function
    std::vector<uint8_t> pixelsToLDR(float gamma = 1.0f) {
        std::vector<uint8_t> data;
        data.reserve(3 * _width * _height); // reserve memory but don't initialize it, so we can use push_back
        for (Color& color : _pixels) {
            data.emplace_back(255 * std::pow(color.r, gamma));
            data.emplace_back(255 * std::pow(color.g, gamma));
            data.emplace_back(255 * std::pow(color.b, gamma));
        }

        return data; // I think this moves the vector without copying it automatically?
    }

    void writePFM(std::string fileName) {
        std::ofstream output(fileName, std::ios::binary);

        // write header, always little endian
        output << "PF\n" << _width << " " << _height << "\n-1.0\n";

        // write pixels
        for (int j = _height - 1; j >= 0; j--) {
            for (int i = 0; i < _width; i++) {
                auto pixel = getPixel(i, j);
                writeFloat(output, pixel.r, Endianness::LITTLE);
                writeFloat(output, pixel.g, Endianness::LITTLE);
                writeFloat(output, pixel.b, Endianness::LITTLE);
            }
        }
    }

    void writePNG(std::string fileName, float gamma) {
        // to use stbi_write we need to convert string to char* and vector to pointer
        // 3 * width is the distance in bytes between the first byte of a row and
        // the first one of the next, 1 byte per uint8, 3 floats per color
        stbi_write_png(fileName.c_str(), _width, _height, 3, &pixelsToLDR(gamma)[0], 3 * _width);
    }

    void writeJPG(std::string fileName, float gamma) {
        // the last parameter is image quality, 100 is max
        stbi_write_jpg(fileName.c_str(), _width, _height, 3, &pixelsToLDR(gamma)[0], 100);
    }
};

#endif