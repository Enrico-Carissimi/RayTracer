#include "Camera.hpp"
#include "World.hpp"
#include "renderers.hpp"
#include "Scenefile.hpp"
#include "lib/CLI11.hpp"



/**
 * @brief validate a string in the format "name:value", where value is convertible to float
 * 
 * @param s string to validate
 * @param floatVariables container for name-value pairs
 */
void validateFloatVariable(std::string& s, std::unordered_map<std::string, float>& floatVariables) {
    std::string key;
    float value;

    size_t position = s.find(':');
    if (position == std::string::npos)
        throw std::invalid_argument("ERROR: \"" + s + "\" does not define a float variable\n"
                                    "the correct syntax is --float=name:value");

    key = s.substr(0, position);

    auto stringVal = s.erase(0, position + 1);
    try { value = std::stof(stringVal); }
    catch (std::out_of_range& e) { throw std::out_of_range(stringVal + " is out of float range"); }
    catch (std::invalid_argument& e) { throw std::invalid_argument(stringVal + " is not a valid number"); }

    floatVariables[key] = value;
}

// Render command to generate images from scene files
void render(const  std::string& input, const std::string& output, int width, float aspectRatio, float a, float gamma, const std::vector<std::string>& floatBuffer,
            const std::string& algorithm, int AAsamples, int nRays, int maxDepth, int russianRouletteLimit) {
    std::ifstream file(input);
    if (file.fail()) { // we already check using CLI11 but you never know
        std::cout << "ERROR: impossible to open file \"" + input + "\"" << std::endl;
        exit(-1);
    }

    std::unordered_map<std::string, float> floatVariables;
    for (auto s : floatBuffer) {
        validateFloatVariable(s, floatVariables);
    }

    Scene scene;
    InputStream stream(file, input);

    scene.parseScene(stream, floatVariables);
    if (scene.camera == nullptr) // default camera
        scene.camera = std::make_shared<Camera>("perspective", 1., 100., 1., translation(-1., 0., 0.));

    // reshape the image from terminal
    if (aspectRatio > 0.) scene.camera->aspectRatio = aspectRatio;
    if (width > 0) scene.camera->imageWidth = width;
    if (width > 0 || aspectRatio > 0.) {
        scene.camera->imageHeight = scene.camera->imageWidth / scene.camera->aspectRatio;
        scene.camera->image = HDRImage(scene.camera->imageWidth, scene.camera->imageHeight);
    }

    if (algorithm == "path")
        scene.camera->render(Renderers::PathTracer, AAsamples, scene.world, scene.camera->pcg, nRays, maxDepth, russianRouletteLimit);
    else if (algorithm == "onoff")
        scene.camera->render(Renderers::OnOff, AAsamples, scene.world);
    else if (algorithm == "flat")
        scene.camera->render(Renderers::Flat, AAsamples, scene.world);
    else if (algorithm == "light")
        scene.camera->render(Renderers::PointLight, AAsamples, scene.world);
    else {
        std::cout << "ERROR: \"" + algorithm + "\" is not a supported rendering algorithm\n" +
                     "supported algorithms are: \"path\", \"onoff\", \"flat\", \"light\", see --help for more information" << std::endl;
        exit(-1);
    }

    scene.camera->image.save(std::filesystem::path(output).stem().string() + ".pfm"); // saves the rendered pfm
    scene.camera->image.normalize(a);
    scene.camera->image.clamp();
    scene.camera->image.save(output, gamma);
}



int main(int argc, char* argv[]) {
    CLI::App app{"RayTracer CLI - convert or render"};

    std::string inputFile = "in.pfm", outputFile = "image.png";
    float a = 1.0f, gamma = 1.0f;

    // Convert Command
    auto convertCommand = app.add_subcommand("convert", "Convert a .pfm file to another format");
    convertCommand->add_option("input,-i,--input", inputFile, "Input .pfm file")->required()->check(CLI::ExistingPath);
    convertCommand->add_option("a,-a,--normalization", a, "Normalization factor, defaults to 1")->required()->check(CLI::PositiveNumber);
    convertCommand->add_option("gamma,-g,--gamma", gamma, "Gamma correction, defaults to 1")->required()->check(CLI::PositiveNumber);
    convertCommand->add_option("output,-o,--output", outputFile, "Output image file")->required();

    // Render Command
    int nRays = 3, maxDepth = 5, russianRouletteLimit = 3, AAsamples = 4;
    std::string algorithm = "path";
    int imageWidth = 0;
    float aspectRatio = 0.;
    std::vector<std::string> floatBuffer{};
    std::unordered_map<std::string, float> floatVariables;

    auto renderCommand = app.add_subcommand("render", "Generate a ray-traced image");
    renderCommand->add_option("input,-i,--input", inputFile, "Input .txt file describing the scene to render")->required()->check(CLI::ExistingPath);
    renderCommand->add_option("output,-o,--output", outputFile, "Output file for the rendered .png or .jpeg image, a .pfm image is always saved");
    renderCommand->add_option("-w,--width", imageWidth, "Width of the output image in pixels, overwrites the one defined for the camera")->check(CLI::PositiveNumber);
    renderCommand->add_option("-r,--aspect-ratio", aspectRatio, "Aspect ratio of the output image, overwrites the one defined for the camera")->check(CLI::PositiveNumber);
    renderCommand->add_option("-a,--norm", a, "Output image normalization factor, defaults to 1")->check(CLI::PositiveNumber);
    renderCommand->add_option("-g,--gamma", gamma, "Output image gamma correction, defaults to 1")->check(CLI::PositiveNumber);
    renderCommand->add_option("-A,--AA-samples", AAsamples, "Number of samples per pixel used for anti-aliasing")->check(CLI::PositiveNumber);
    renderCommand->add_option("-n,--ray-number", nRays, "Path tracer only, number of rays sent from every hit point")->check(CLI::PositiveNumber);
    renderCommand->add_option("-d,--max-depth", maxDepth, "Path tracer only, maximum ray depth")->check(CLI::PositiveNumber);
    renderCommand->add_option("-l,--rr-limit", russianRouletteLimit, "Path tracer only, ray depth where russian roulette starts")->check(CLI::NonNegativeNumber);
    renderCommand->add_option("-R,--algo", algorithm, "Algorithm to use for rendering: \"path\" (path tracing), \"onoff\", \"flat\", \"light\" (point light tracer)")->check(CLI::IsMember({"path", "onoff", "flat", "light"}));
    renderCommand->add_option("-f,--float", floatBuffer, "Declare named float variables, overwrites the ones with the same name in the input file. Syntax: name:value");



    CLI11_PARSE(app, argc, argv);

    if (*convertCommand) {
        HDRImage image(inputFile);
        image.normalize(a);
        image.clamp();
        image.save(outputFile, gamma);
    }
    else if (*renderCommand) {
        render(inputFile, outputFile, imageWidth, aspectRatio, a, gamma, floatBuffer, algorithm, AAsamples, nRays, maxDepth, russianRouletteLimit);
    }
    else {
        std::cout << "Program usage: " << argv[0] << " [demo or convert]\n"
                  << "Run with --help for more information." << std::endl; 
    }

    return 0;
}