#ifndef __PFMReader__
#define __PFMReader__

#include <iostream>
#include <string>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <sstream>

enum class Endianness {LITTLE, BIG};

/**
 * @brief Reads a float value from a stream considering the specified endianness.
 * 
 * @param stream Input stream to read 4 bytes from.
 * @param endianness Byte order (LITTLE or BIG endian).
 * @return float
 * @throws std::runtime_error If unable to read 4 bytes.
 */
float readFloat(std::istream& stream, Endianness endianness);

/**
 * @brief Writes a float to a stream with specified endianness.
 * 
 * This function is adapted from Prof. Tomasi's lecture code style.
 *
 * @param stream Output stream to write to.
 * @param value Float value to write.
 * @param endianness Byte order (LITTLE or BIG endian).
 */
void writeFloat(std::ostream& stream, float value, Endianness endianness);

/**
 * @brief Reads a full line from the input stream.
 * 
 * @param stream Input stream.
 * @return std::string
 * @throws std::runtime_error If reading fails.
 */
std::string readLine(std::istream& stream);

/**
 * @brief Parses the image size (width and height) from a line of text.
 * 
 * @param line String containing two integers (width and height).
 * @return std::pair<int, int>
 * @throws std::invalid_argument If the line does not contain two positive integers.
 * @throws std::runtime_error If the line contains extra values.
 */
std::pair<int, int> parseImageSize(const std::string& line);

/**
 * @brief Parses the endianness from a line containing a float value.
 * 
 * @param line String representing the scale factor (endianness).
 * @return Endianness
 * @throws std::invalid_argument If the line is not a valid float or zero.
 */
enum Endianness parseEndianness(const std::string& line);

#endif
