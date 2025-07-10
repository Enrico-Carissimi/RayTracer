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
#include "utils.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION // needed for stb
#include "lib/stb_image_write.h"



/**
 * @class HDRImage
 * @brief Represents a high dynamic range (HDR) image with floating-point color data.
 */
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

    int pixelIndex(int i, int j) const {
        return i + _width * j;
    }

    bool validCoordinates(int i, int j) const {
        return (i >= 0 && i < _width && j >= 0 && j < _height);
    }

    void checkCoordinates(int i, int j) const {
        if (!validCoordinates(i, j)) {
            throw std::invalid_argument("ERROR: trying to access invalid image coordinates (" + std::to_string(i) + ", " + std::to_string(j) + "), "
                                        + "image size is " + std::to_string(_width) + " x " + std::to_string(_height));
        }
        return;
    }

    Color getPixel(int i, int j) const {
        checkCoordinates(i, j);
        return _pixels[pixelIndex(i, j)];
    }

    void setPixel(int i, int j, Color color) {
        checkCoordinates(i, j);
        _pixels[pixelIndex(i, j)] = color;
        return;
    }

    /**
     * @brief Computes the average luminosity of the entire image.
     * 
     * Uses logarithmic averaging to avoid skew by very bright pixels.
     * 
     * @param delta Small constant added to avoid log(0).
     * @return float
     */
    float averageLuminosity(float delta = 1e-10f) {
        float sum = 0.0f;
        for (const Color& pixel : _pixels) {
            sum += std::log10(pixel.luminosity() + delta);
        }

        sum /= _pixels.size();

        return std::pow(10.0f, sum);
    }

    void normalize(float a, float luminosity = 0.0f) {
        if (luminosity == 0.0f) {
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

    /**
     * @brief Saves the image to a file, with format chosen by the extension (.pfm, .png, .jpg/.jpeg).
     * 
     * @param fileName Output file path.
     * @param gamma Gamma correction to apply (default 1.0).
     * @throws std::invalid_argument if the extension is unsupported.
     */
    void save(std::string fileName, float gamma = 1.0f) {
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

    /**
     * @brief Converts HDR pixel data to low dynamic range (LDR) 8-bit per channel format.
     * 
     * Must normalize pixels before calling.
     * 
     * @param gamma Gamma correction value (default 1.0).
     * @return std::vector<uint8_t> Vector of bytes representing the LDR pixel data.
     */
    std::vector<uint8_t> pixelsToLDR(float gamma = 1.0f) {
        int length = _width * _height;
        std::vector<uint8_t> data(3 * length);
        for (int i = 0; i < length; i++) {
            data[3 * i] = (255 * std::pow(_pixels[i].r, gamma));
            data[3 * i + 1] = (255 * std::pow(_pixels[i].g, gamma));
            data[3 * i + 2] = (255 * std::pow(_pixels[i].b, gamma));
        }

        return data;
    }

    /**
     * @brief Writes the HDR image to a PFM file.
     * 
     * Uses little endian byte order.
     * 
     * @param fileName The output PNG file path.
     */
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

    /**
    * @brief Writes the HDR image to a PNG file applying gamma correction.
    * 
    * Uses the stb_image_write library to save the image.
    * Converts the internal HDR floating-point pixel data to 8-bit per channel LDR format
    * with gamma correction before writing.
    * 
    * @param fileName The output PNG file path.
    * @param gamma The gamma correction value to apply (default is 1.0, meaning no correction).
    */
    void writePNG(std::string fileName, float gamma = 1.0f) {
        stbi_write_png(fileName.c_str(), _width, _height, 3, &pixelsToLDR(gamma)[0], 3 * _width);
    }

    /**
    * @brief Writes the HDR image to a JPEG file applying gamma correction.
    * 
    * Uses the stb_image_write library to save the image.
    * Converts the internal HDR floating-point pixel data to 8-bit per channel LDR format
    * with gamma correction before writing.
    * 
    * @param fileName The output PNG file path.
    * @param gamma The gamma correction value to apply (default is 1.0, meaning no correction).
    */
    void writeJPG(std::string fileName, float gamma = 1.0f) {
        // the last parameter is image quality, 100 is max
        stbi_write_jpg(fileName.c_str(), _width, _height, 3, &pixelsToLDR(gamma)[0], 100);
    }
};

#endif