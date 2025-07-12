#include <iostream>
#include "materials.hpp"
#include "utils.hpp"

void testUniformTexture() {
    Color color(1., 2., 3.);
    UniformTexture texture(color);

    sassert(texture.color(Vec2(0., 0.)).isClose(color));
    sassert(texture.color(Vec2(1., 0.)).isClose(color));
    sassert(texture.color(Vec2(0., 1.)).isClose(color));
    sassert(texture.color(Vec2(1., 1.)).isClose(color));

    std::cout << "uniform texture works" << std::endl;
}

void testImageTexture(){
    HDRImage image(2, 2);
    image.setPixel(0, 0, Color(1., 2., 3.));
    image.setPixel(1, 0, Color(2., 3., 1.));
    image.setPixel(0, 1, Color(2., 1., 3.));
    image.setPixel(1, 1, Color(3., 2., 1.));

    ImageTexture texture(image);
    sassert(texture.color(Vec2(0., 0.)).isClose(Color(1., 2., 3.)));
    sassert(texture.color(Vec2(1., 0.)).isClose(Color(2., 3., 1.)));
    sassert(texture.color(Vec2(0., 1.)).isClose(Color(2., 1., 3.)));
    sassert(texture.color(Vec2(1., 1.)).isClose(Color(3., 2., 1.)));

    std::cout << "texture from image works" << std::endl;
}

void testCheckeredTexture(){
    Color color1(1., 2., 3.);
    Color color2(10., 20., 30.);

    CheckeredTexture texture(color1, color2, 2);

    // With nSteps == 2, the pattern should be the following:
    //
    //              (0.5, 0)
    //   (0, 0) +------+------+ (1, 0)
    //          |      |      |
    //          | col1 | col2 |
    //          |      |      |
    // (0, 0.5) +------+------+ (1, 0.5)
    //          |      |      |
    //          | col2 | col1 |
    //          |      |      |
    //   (0, 1) +------+------+ (1, 1)
    //              (0.5, 1)
    sassert(texture.color(Vec2(0.25, 0.25)).isClose(color1));
    sassert(texture.color(Vec2(0.75, 0.25)).isClose(color2));
    sassert(texture.color(Vec2(0.25, 0.75)).isClose(color2));
    sassert(texture.color(Vec2(0.75, 0.75)).isClose(color1));

    std::cout << "checkered texture works" << std::endl;
}



int main() {
    testUniformTexture();
    testImageTexture();
    testCheckeredTexture();

    return 0;
}