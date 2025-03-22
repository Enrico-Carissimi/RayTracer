#include "PFMReader.hpp"
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <sstream>

float read_float(std::istream &stream, bool little_endian) {
    uint8_t bytes[4];  
    stream.read(reinterpret_cast<char*>(bytes), 4);  

    if (stream.gcount() != 4) {  
        throw std::runtime_error("Error: impossible read 4 byte");
    }

    uint32_t int_rep;
    if (little_endian) {
        int_rep = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
    } else {
        int_rep = bytes[3] | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24);
    }

    float result;
    std::memcpy(&result, &int_rep, sizeof(float));  
    return result;
}

std::string read_line(std::istream &stream) {
    std::string line;
    char ch;

    while (stream.get(ch)) {  // Reads one character at a time
        if (ch == '\n') {  
            break;  // If it finds '\n', it exits the loop
        }
        line += ch;  // Adds the character to the string
    }

    if (line.empty() && stream.eof()) {  
        throw std::runtime_error("Error: end of file reached with no data");
    }

    return line;
}

std::pair<int, int> parse_img_size(const std::string &line) {
    std::istringstream iss(line);
    int width, height;
    
    if (!(iss >> width >> height)) {
        throw std::runtime_error("Invalid image size format");
    }

    return {width, height};
}


bool parse_endianness(const std::string &line) {
    float scale;
    try {
        scale = std::stof(line);
    } catch (const std::exception &) {
        throw std::runtime_error("Invalid endianness format: " + line);
    }

    if (scale == 1.0f) {
        return true;  // Big-endian
    } else if (scale == -1.0f) {
        return false; // Little-endian
    } else {
        throw std::runtime_error("Unexpected scale factor: " + line);
    }
}

