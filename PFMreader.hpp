#ifndef __PFMreader__
#define __PFMreader__

#include <iostream>
#include <string>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <sstream>

#include "HDRImage.hpp"

enum class Endianness {LITTLE, BIG};

float readFloat(std::istream& stream, Endianness endianness) {
    uint8_t bytes[4];
    stream.read(reinterpret_cast<char*>(bytes), 4);

    if (stream.gcount() != 4) {  
        throw std::runtime_error("ERROR: impossible to read 4 bytes");
    }

    uint32_t intBuffer;
    switch (endianness) {
    case Endianness::LITTLE:
        intBuffer = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24); break;
    case Endianness::BIG:
        intBuffer = bytes[3] | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24); break;
    }

    float result;
    std::memcpy(&result, &intBuffer, sizeof(float));  
    return result;
}

std::string readLine(std::istream& stream){
    std::string buffer;
    if (!std::getline(stream, buffer)) throw std::runtime_error("ERROR: impossible to read line");
    return buffer;
}

std::pair<int, int> parseImageSize(const std::string& line) {
    std::istringstream iss(line);
    int width, height;
    
    if (!(iss >> width >> height)) {
        throw std::invalid_argument("ERROR: invalid image size format, 2 int expected");
    }
    if (width <= 0 || height <= 0) {
        throw std::invalid_argument("ERROR: image dimensions must be positive integers");
    }
    std::string buffer;
    if (iss >> buffer) {
        throw std::runtime_error("ERROR: too many values in size line");
    }

    return {width, height};
}

enum Endianness parseEndianness(const std::string& line) {
    float endianness;
    try {
        endianness = std::stof(line);
    } catch (const std::exception& e) {
        std::cout << "ERROR: invalid endianness format \"" + line + "\", must be a float" << std::endl;
    }

    if (endianness > 0.0f) return Endianness::BIG;
    if (endianness < 0.0f) return Endianness::LITTLE;
    throw std::invalid_argument("ERROR: endianness must be non-zero");
}

#endif

