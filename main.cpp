#include <iostream>
#include <fstream>
#include <vector>
#include "HDRImage.hpp"

const unsigned int EXPECTED_ARGS = 4;

void parseCommandLine(int argc, char* argv[], std::string& input, float& factor, float& gamma, std::string& output) {
    if (argc != EXPECTED_ARGS + 1)
        throw std::invalid_argument("ERROR: program expects " + std::to_string(EXPECTED_ARGS) + " arguments, "
                                    + std::to_string(argc - 1) + " where given\nusage: ./main <input> <a> <gamma> <output>");

    input = argv[1];

    try {
        factor = std::stof(argv[2]);
    } catch (const std::exception&) {
        throw std::runtime_error("ERROR: factor must be a floating-point number");
    }

    try {
        gamma = std::stof(argv[3]);
    } catch (const std::exception&) {
        throw std::runtime_error("ERROR: gamma must be a floating-point number");
    }

    output = argv[4];
}



int main(int argc, char* argv[]) {
    float a, gamma; // !!! they must be positive too !!!
    std::string inputFile, outputFile;

    parseCommandLine(argc, argv, inputFile, a, gamma, outputFile);

    HDRImage image(argv[1]);
    image.normalize(a);
    image.clamp();
    image.save(argv[4]); // add error message if it fails to save?

    return 0;
}
