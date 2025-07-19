# Writing scene files

This program reads the scene to render from a text file, with the syntax described below. Float variables can be defined and used afterwards, materials _must_ be defined before use. You can check the examples in this folder for reference.

- Camera: camera(type, [float] aspect ratio, [int] image width, [float] distance, [transformation]). Valid camera types are "orthogonal" and "perspective". For the orthogonal camera "distance" is ignored, you can set it to an arbitrary value. If the camera is not defined, a default one is used.
- Float variables: float identifier([float] value). You can declare a variable in the scene file and change its value from the terminal, using the -f or --float option: -f identifier:value.
- Strings: "text"
- Colors: <[float] r, [float] g, [float] b>
- Vectors: [[float] x, [float] y, [float] z]
- Transformations:
    - Scaling: scaling([vector] scale factors). To scale uniformly, you need a vector like [scale, scale, scale];
    - Translation: translation([vector]);
    - Rotations: rotationX([float] angle), same for rotationY and rotationZ around the three axes.

    Transformations can be multiplied with '*'.
- Textures:
    - Uniform: uniform([color]);
    - Checkered: checkered([color] first, [color] second, [float] number of step);
    - From image: image([string] file name). The file must be a .pfm image, paths are relative to the executable.
- Materials:
    - Diffuse: material identifier(diffuse([texture], [texture] emitted radiance));
    - Reflective: material identifier(specular([texture], [texture] emitted radiance, [float] blur)), where "blur" is optional, 0 if omitted;
    - Transparent: material identifier(transparent([texture], [texture] emitted radiance, [float] refraction index)), where the refraction index must divided by the one of the outside material.
- Shapes: type([material], [transformation]). Valid types are "sphere" and "plane". The material here is a material identifier, the material itself must be defined outside the shape definition.
- Point lights (for the point light renderer): pointLight([vector] position, [color], [float] radius).
- Comments start with '#'.