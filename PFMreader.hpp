#ifndef PFMREADER_HPP
#define PFMREADER_HPP

#include <iostream>
#include <string>

// Reads a 32-bit float from a binary stream, considering endianness
float read_float(std::istream &input, bool big_endian);

// Reads a line of text up to '\n' or the end of the stream
std::string read_line(std::istream &input);

// Parse image dimensions from a string
std::pair<int, int> parse_img_size(const std::string &line);

// Determine endianness from a string
bool parse_endianness(const std::string &line);

#endif // PFMREADER_HPP

