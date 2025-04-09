#ifndef PARAMETERS_HPP
#define PARAMETERS_HPP

#include <string>
#include <stdexcept>
#include <vector>

class Parameters {
public:
    std::string input_pfm_file_name;
    float factor = 0.2f;
    float gamma = 1.0f;
    std::string output_png_file_name;

    void parse_command_line(const std::vector<std::string>& argv) {
        if (argv.size() != 5) {
            throw std::runtime_error("Usage: main INPUT_PFM_FILE FACTOR GAMMA OUTPUT_PNG_FILE");
        }

        input_pfm_file_name = argv[1];

        try {
            factor = std::stof(argv[2]);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid factor ('" + argv[2] + "'), it must be a floating-point number.");
        }

        try {
            gamma = std::stof(argv[3]);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid gamma ('" + argv[3] + "'), it must be a floating-point number.");
        }

        output_png_file_name = argv[4];
    }
};

#endif // PARAMETERS_HPP
