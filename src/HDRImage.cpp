#include "HDRImage.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION // needed ONCE for stb
#include "stb_image_write.h"

float HDRImage::averageLuminosity(float delta) {
    float sum = 0.0f;
    for (const Color& pixel : _pixels) {
        sum += std::log10(pixel.luminosity() + delta);
    }

    sum /= _pixels.size();

    return std::pow(10.0f, sum);
}

void HDRImage::normalize(float a, float luminosity) {
    if (luminosity == 0.0f) {
        luminosity = averageLuminosity();
    }

    // calculate the scale factor
    float scale = a / luminosity;

    for (Color& pixel: _pixels) {
        pixel = pixel * scale;
    }
}

void HDRImage::readPFM(std::istream& input) {
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

std::vector<uint8_t> HDRImage::pixelsToLDR(float gamma) {
    int length = _width * _height;
    std::vector<uint8_t> data(3 * length);
    float invGamma = 1.0f / gamma;
    
    for (int i = 0; i < length; i++) {
        data[3 * i] = (255 * std::pow(_pixels[i].r, invGamma));
        data[3 * i + 1] = (255 * std::pow(_pixels[i].g, invGamma));
        data[3 * i + 2] = (255 * std::pow(_pixels[i].b, invGamma));
    }

    return data;
}

void HDRImage::writePFM(std::string fileName) {
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