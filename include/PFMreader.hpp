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

/**
 * @brief Reads a float value from a stream considering the specified endianness.
 * 
 * @param stream Input stream to read 4 bytes from.
 * @param endianness Byte order (LITTLE or BIG endian).
 * @return float The float value read from the stream.
 * @throws std::runtime_error If unable to read 4 bytes.
 */
float readFloat(std::istream& stream, Endianness endianness) {
    uint8_t bytes[4];
    stream.read(reinterpret_cast<char*>(bytes), 4);

    if (stream.gcount() != 4) {  
        throw std::runtime_error("ERROR: impossible to read 4 bytes, only " + std::to_string(stream.gcount()) + " available");
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

/**
 * @brief Writes a float to a stream with specified endianness.
 * 
 * This function is adapted from Prof. Tomasi's lecture code style.
 *
 * @param stream Output stream to write to.
 * @param value Float value to write.
 * @param endianness Byte order (LITTLE or BIG endian).
 */
void writeFloat(std::ostream& stream, float value, Endianness endianness) {
    // Convert "value" in a sequence of 32 bit
    uint32_t doubleWord{*((uint32_t*) &value)};
  
    // Extract the four bytes in "doubleWord" using bit-level operators
    uint8_t bytes[] = {
        static_cast<uint8_t>(doubleWord & 0xff),         // Least significant byte
        static_cast<uint8_t>((doubleWord >> 8) & 0xff),
        static_cast<uint8_t>((doubleWord >> 16) & 0xff),
        static_cast<uint8_t>((doubleWord >> 24) & 0xff), // Most significant byte
    };
  
    switch (endianness) {
    case Endianness::LITTLE:
        for (int i{}; i < 4; ++i) stream << bytes[i]; break; // Forward loop   
    case Endianness::BIG:
        for (int i{3}; i >= 0; --i) stream << bytes[i]; break; // Backward loop
    }
}

/**
 * @brief Reads a full line from the input stream.
 * 
 * @param stream Input stream.
 * @return std::string The line read as a string.
 * @throws std::runtime_error If reading fails.
 */
std::string readLine(std::istream& stream) {
    std::string buffer;
    if (!std::getline(stream, buffer)) throw std::runtime_error("ERROR: impossible to read line");
    return buffer;
}

/**
 * @brief Parses the image size (width and height) from a line of text.
 * 
 * @param line String containing two integers (width and height).
 * @return std::pair<int, int> Pair representing (width, height).
 * @throws std::invalid_argument If the line does not contain two positive integers.
 * @throws std::runtime_error If the line contains extra values.
 */
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

/**
 * @brief Parses the endianness from a line containing a float value.
 * 
 * @param line String representing the scale factor (endianness).
 * @return Endianness LITTLE or BIG endian based on the float value.
 * @throws std::invalid_argument If the line is not a valid float or zero.
 */
enum Endianness parseEndianness(const std::string& line) {
    float endianness;
    try {
        endianness = std::stof(line);
    } catch (const std::exception& e) {
        throw std::invalid_argument("ERROR: invalid endianness format \"" + line + "\", must be a float");
    }

    if (endianness > 0.0f) return Endianness::BIG;
    if (endianness < 0.0f) return Endianness::LITTLE;
    throw std::invalid_argument("ERROR: endianness must be non-zero");
}

#endif