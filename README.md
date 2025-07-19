# RayTracer
This project is a simple ray tracer.\
It can render scenes using various ray tracing algorithms and can convert PFM images to PNG or JPEG.

## Installation
Requires c++20, a compiler that supports it, and CMake 3.31.6+.\
Download the release for the latest version from [here](https://github.com/Enrico-Carissimi/RayTracer/releases).\
To build the project, create a "build" directory inside "RayTracer" and from that simply call:
```
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```
To run unit tests, simply do:
```
ctest
```
If you prefer to work in the source directory, the three commands above become:
```
cmake -DCMAKE_BUILD_TYPE=Release . -B build
cmake --build build
ctest --test-dir build
```

Use option `-G` in the first command to manually select the generator.

## Usage
In the following, the name of the executable is "RayTracer".

### Convert
To convert images, do:
```
RayTracer convert <input PFM> <normalization> <gamma> <output PNG or JPEG> [parameters...]
```
The only available parameter is the image luminosity, adjustable with `--luminosity`.

### Render
This ray tracer can render images using four different algorithms:
- a path tracer, with russian roulette, for photorealistic images (the parameters are tunable);
- a "flat" renderer, that estimates the solution of the rendering equation using only the textures of the objects, neglecting any contributions of the light;
- an "on-off" renderer, that renders only the shapes of the objects without any color: fast and useful for debug;
- a simple point-light renderer, but different materials are not very well supported for now.

To render a scene described in an input file, do:
```
RayTracer render <input scene file> [output] [parameters...]
```
The default output is "image.png". You can choose the algorithm used to render the image with `--algo` (options are "path", "flat", "onoff", "light"), and you can tune the number of samples used for anti-aliasing (`--AA-samples`), the size of the image (`--width` and `--aspect-ratio`), the parameters of the path tracer, and more. Use `--help` for more information. Most options have a shorthand version.

You can quickly create a low-quality demo image with:
```
RayTracer render examples/demo.txt -A 1 -n 1 
```
If you want a higher quality demo, with `-A 4 -n 3` it's not that slow and a lot better.

### Scene files
The scene to render must be defined in a text file, see [examples/README.md](https://github.com/Enrico-Carissimi/RayTracer/blob/main/examples/README.md) for more information.

## History
See [CHANGELOG.md](https://github.com/Enrico-Carissimi/RayTracer/blob/main/CHANGELOG.md).

## License
RayTracer is available for free under the [EUPL-1.2 license](https://eupl.eu/), see [LICENSE.md](https://github.com/Enrico-Carissimi/RayTracer/blob/main/LICENSE.md).
