#include <iostream>
#include "HDRImage.hpp"
#include "PFMreader.hpp"
#include "Color.hpp"
#include "Camera.hpp"
#include "HitRecord.hpp"
#include "Normal3.hpp"
#include "Parameters.hpp"
#include "Point3.hpp"
#include "Ray.hpp"
#include "shapes.hpp"
#include "Transformation.hpp"
#include "utils.hpp"
#include "Vec2.hpp"
#include "Vec3.hpp"
#include "World.hpp"
#include "CLI11.hpp"

// Funzione demo per generare un'immagine a scacchiera
HDRImage demo() {
    int width = 512, height = 512;
    HDRImage image(width, height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool white = ((x / 64 + y / 64) % 2 == 0);
            image.setPixel(x, y, white ? Color(1, 1, 1) : Color(0, 0, 0));
        }
    }

    return image;
}

int main(int argc, char* argv[]) {
    CLI::App app{"RayTracer CLI - convert or demo mode"};

    std::string inputFile, outputFile;
    float a = 1.0f, gamma = 2.2f;

    //Convert Command
    auto convert_cmd = app.add_subcommand("convert", "Convert a .pfm file to another format");
    convert_cmd->add_option("input", inputFile, "Input .pfm file")->required();
    convert_cmd->add_option("a", a, "Normalization factor")->required();
    convert_cmd->add_option("gamma", gamma, "Gamma correction")->required();
    convert_cmd->add_option("output", outputFile, "Output image file")->required();

    //Convert Command
    std::string demoOutput;
    auto demo_cmd = app.add_subcommand("demo", "Generate a demo ray-traced image");
    demo_cmd->add_option("output", demoOutput, "Output file for the demo")->required();

    CLI11_PARSE(app, argc, argv);

    if (*convert_cmd) {
        HDRImage image(inputFile);
        image.normalize(a);
        image.clamp();
        image.save(outputFile, gamma);
    }
    else if (*demo_cmd) {
        HDRImage demoImage = demo();
        demoImage.clamp();
        demoImage.save(demoOutput, 2.2f);
    }

    return 0;
}
