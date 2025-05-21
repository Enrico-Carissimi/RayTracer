# RayTracer
This project is a simple ray tracer.\
For now, it can convert PFM images to PNG or JPEG and output a simple demo image.

## Installation and build
Requires c++20.\
Download the release for version 0.2.0.\
Create a build directory inside RayTracer and from that simply call
```
cmake ..
cmake --build .
```
## Usage
Calling RayTracer the executable:\
to convert images, do
```
RayTracer convert [input] [normalization] [gamma] [output]
```
to see the demo, do
```
RayTracer demo [output] [angle]
```
the parameters are optional, use --help for more information.
