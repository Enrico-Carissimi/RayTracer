# RayTracer
This project is a simple ray tracer.\
For now, it can convert PFM images to PNG or JPEG and output a simple demo image using a path tracer with russian roulette.

## Installation and build
Requires c++20 (and a compiler that supports it) and CMake 3.31.6+.\
Download the release for version 0.3.0.\
Create a build directory inside RayTracer and from that simply call:
```
cmake ..
cmake --build .
```
To run unit tests, simply do:
```
ctest
```
If you prefer to work in the source directory, the three commands above become:
```
cmake -B build
cmake --build build
ctest --test-dir build
```

## Usage
If RayTracer is the name of the executable:\
to convert images, do:
```
RayTracer convert <input PFM> [normalization] [gamma] <output PNG or JPEG>
```
to print the demo image, do:
```
RayTracer demo [output] [parameters...]
```
default output is "demo.png". You can tune the parameters of the path tracer and the number of samples used for anti-aliasing, use --help for more information.
