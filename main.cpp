#include <iostream>
#include <fstream>
#include <vector>
#include "Parameters.hpp"
#include "HDRImage.hpp"
#include "PFMReader.hpp"

int main(int argc, char* argv[]) {
    Parameters parameters;
    
    try {
        // Converto argv in un vettore di stringhe per facilitarne la gestione
        std::vector<std::string> args(argv, argv + argc);
        parameters.parse_command_line(args);
    } catch (const std::exception& err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return 1;
    }

    try {
        // Read PFM Image
        std::ifstream inputFile(parameters.input_pfm_file_name, std::ios::binary);
        if (!inputFile) {
            throw std::runtime_error("Error: Cannot open input file.");
        }

        HDRImage image(argv[1]);
        inputFile.close();

        std::cout << "File \"" << parameters.input_pfm_file_name << "\" has been read from disk.\n";

        // Normalize the Image
        image.normalizeImage(parameters.factor);

        // Clamping 
        image.clampImage();

        // Scrivo l'immagine LDR (PNG)
        std::ofstream outputFile(parameters.output_png_file_name, std::ios::binary);
        if (!outputFile) {
            throw std::runtime_error("Error: Cannot open output file.");
        }
        // Serve la funzione che scrive
        //img.write_ldr_image(outputFile, "PNG", parameters.gamma);
        outputFile.close();
        
        std::cout << "File \"" << parameters.output_png_file_name << "\" has been written to disk.\n";

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
