#include "Camera.hpp"
#include "World.hpp"
#include "lib/CLI11.hpp"

// Demo command to generate an example image
void demo(std::string output, float angle) {
    Camera camera("perspective", 4. / 3., 400, 1., rotation(angle, Axis::Z) * translation(-1., 0., 0.));
    World world;

    int nSpheres = 10;
    Vec3 positions[] = {
        Vec3{-0.5, -0.5, -0.5},
        Vec3{-0.5, -0.5, 0.5},
        Vec3{-0.5, 0.5, -0.5},
        Vec3{0.5, -0.5, -0.5},
        Vec3{-0.5, 0.5, 0.5},
        Vec3{0.5, -0.5, 0.5},
        Vec3{0.5, 0.5, -0.5},
        Vec3{0.5, 0.5, 0.5},
        Vec3{0., 0., -0.5},
        Vec3{0., 0.5, 0.}
    };
    
    for (int i = 0; i < nSpheres; i++) {
        world.addShape(std::make_shared<Sphere>(Sphere(translation(positions[i]) * scaling(0.1))));
    }

    camera.castAll([&world](Ray ray){
        HitRecord rec;
        return world.isHit(ray, rec) ? Color(1., 1., 1.) : Color(0., 0., 0.); 
    });
    camera.image.save("demo.pfm");
    camera.image.normalize(1.);
    camera.image.clamp();
    camera.image.save(output);
}



int main(int argc, char* argv[]) {
    CLI::App app{"RayTracer CLI - convert or demo mode"};

    // Convert Command
    std::string inputFile, outputFile;
    float a = 1.0f, gamma = 1.0f;

    auto convertCommand = app.add_subcommand("convert", "Convert a .pfm file to another format");
    convertCommand->add_option("input,-i,--input", inputFile, "Input .pfm file")->required();
    convertCommand->add_option("a,-a,--normalization", a, "Normalization factor, defaults to 1")->required();
    convertCommand->add_option("gamma,-g,--gamma", gamma, "Gamma correction, defaults to 1")->required();
    convertCommand->add_option("output,-o,--output", outputFile, "Output image file")->required();

    // Demo Command
    std::string demoOutput = "demo.png";
    float angle = 0.0f;

    auto demoCommand = app.add_subcommand("demo", "Generate a demo ray-traced image, this always saves a \"demo.pfm\" image");
    demoCommand->add_option("output,-o,--output", demoOutput, "Output file for the demo .png or .jpeg image");
    demoCommand->add_option("--angle", angle, "Angle of the rotation around the Z axis, in degrees");

    CLI11_PARSE(app, argc, argv);



    if (*convertCommand) {
        HDRImage image(inputFile);
        image.normalize(a);
        image.clamp();
        image.save(outputFile, gamma);
    }
    else if (*demoCommand) {
        demo(demoOutput, angle);
    }
    else {
        std::cout << "Program usage: " << argv[0] << " [demo or convert]\n"
                  << "Run with --help for more information." << std::endl; 
    }

    return 0;
}
