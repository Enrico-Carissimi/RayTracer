#include "Camera.hpp"
#include "World.hpp"
#include "renderers.hpp"
#include "lib/CLI11.hpp"

using std::shared_ptr, std::make_shared;

// Demo command to generate an example image
void demo(std::string output, int nRays, int maxDepth, int russianRouletteLimit, int AAsamples, float angle) {
    Camera camera("perspective", 4. / 3., 640, 1., rotation(angle, Axis::Z) * translation(-1., 0., 0.));
    World world;

    auto material = make_shared<DiffuseMaterial>(make_shared<UniformTexture>(Color(1., 0.5, 0.3)));
    auto skyMaterial = make_shared<DiffuseMaterial>(make_shared<UniformTexture>(Color()), make_shared<UniformTexture>(Color(1., 1., 1.)));
    auto reflectMaterial = make_shared<SpecularMaterial>(make_shared<UniformTexture>(Color(0.1, 0.9, 0.4)), make_shared<UniformTexture>(Color()), 1.);
    auto floorMaterial = make_shared<DiffuseMaterial>(make_shared<CheckeredTexture>(Color(0., 1., 1.), Color(1., 0., 1.), 4));

    world.addShape(make_shared<Sphere>(material, translation(0., 0., -0.2) * scaling(0.8)));
    world.addShape(make_shared<Sphere>(skyMaterial, translation(0., 0., 0.) * scaling(1000.)));
    world.addShape(make_shared<Sphere>(reflectMaterial, translation(1., 2., -0.9)));
    world.addShape(make_shared<Plane>(floorMaterial, translation(0., 0., -1.)));

    

    camera.render(Renderers::PathTracer, AAsamples, world, camera.pcg, nRays, maxDepth, russianRouletteLimit);
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
    int nRays = 3, maxDepth = 5, russianRouletteLimit = 3, AAsamples = 4;

    auto demoCommand = app.add_subcommand("demo", "Generate a demo ray-traced image, this always saves a \"demo.pfm\" image");
    demoCommand->add_option("output,-o,--output", demoOutput, "Output file for the demo .png or .jpeg image");
    demoCommand->add_option("-A,--AA-samples", AAsamples, "Number of samples per pixel used for anti-aliasing");
    demoCommand->add_option("--angle", angle, "Angle of the rotation around the Z axis, in degrees");
    demoCommand->add_option("-n,--ray-number", nRays, "Path tracer only, number of rays sent from every hit point");
    demoCommand->add_option("-d,--max-depth", maxDepth, "Path tracer only, maximum ray depth");
    demoCommand->add_option("-r,--rr-limit", russianRouletteLimit, "Path tracer only, ray depth where russian roulette starts");

    CLI11_PARSE(app, argc, argv);



    if (*convertCommand) {
        HDRImage image(inputFile);
        image.normalize(a);
        image.clamp();
        image.save(outputFile, gamma);
    }
    else if (*demoCommand) {
        demo(demoOutput, nRays, maxDepth, russianRouletteLimit, AAsamples, angle);
    }
    else {
        std::cout << "Program usage: " << argv[0] << " [demo or convert]\n"
                  << "Run with --help for more information." << std::endl; 
    }

    return 0;
}