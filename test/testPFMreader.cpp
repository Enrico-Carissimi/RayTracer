#include <cassert>
#include "../HDRImage.hpp"

using std::cout, std::endl;

// This is the content of "reference_le.pfm" (little-endian file)
const unsigned char LE_REFERENCE_BYTES[] = {
    0x50, 0x46, 0x0a, 0x33, 0x20, 0x32, 0x0a, 0x2d, 0x31, 0x2e, 0x30, 0x0a,
    0x00, 0x00, 0xc8, 0x42, 0x00, 0x00, 0x48, 0x43, 0x00, 0x00, 0x96, 0x43,
    0x00, 0x00, 0xc8, 0x43, 0x00, 0x00, 0xfa, 0x43, 0x00, 0x00, 0x16, 0x44,
    0x00, 0x00, 0x2f, 0x44, 0x00, 0x00, 0x48, 0x44, 0x00, 0x00, 0x61, 0x44,
    0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0xa0, 0x41, 0x00, 0x00, 0xf0, 0x41,
    0x00, 0x00, 0x20, 0x42, 0x00, 0x00, 0x48, 0x42, 0x00, 0x00, 0x70, 0x42,
    0x00, 0x00, 0x8c, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xb4, 0x42
};

// This is the content of "reference_be.pfm" (big-endian file)
const unsigned char BE_REFERENCE_BYTES[] = {
    0x50, 0x46, 0x0a, 0x33, 0x20, 0x32, 0x0a, 0x31, 0x2e, 0x30, 0x0a,
    0x42, 0xc8, 0x00, 0x00, 0x43, 0x48, 0x00, 0x00, 0x43, 0x96, 0x00, 0x00,
    0x43, 0xc8, 0x00, 0x00, 0x43, 0xfa, 0x00, 0x00, 0x44, 0x16, 0x00, 0x00,
    0x44, 0x2f, 0x00, 0x00, 0x44, 0x48, 0x00, 0x00, 0x44, 0x61, 0x00, 0x00,
    0x41, 0x20, 0x00, 0x00, 0x41, 0xa0, 0x00, 0x00, 0x41, 0xf0, 0x00, 0x00,
    0x42, 0x20, 0x00, 0x00, 0x42, 0x48, 0x00, 0x00, 0x42, 0x70, 0x00, 0x00,
    0x42, 0x8c, 0x00, 0x00, 0x42, 0xa0, 0x00, 0x00, 0x42, 0xb4, 0x00, 0x00
};

const unsigned char TEST_WRITE_BYTES[] = {
    0x50, 0x46, 0x0a, 0x33, 0x20, 0x32, 0x0a, 0x2d, 0x31, 0x2e, 0x30, 0x0a,
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, // yellow
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, // magenta
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // black
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, // green
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x3f, // white
    0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // red
};

auto streamFromArray(const unsigned char* bytes, int length) {
    std::stringstream ss;
    for (int i = 0; i < length; i++) ss << bytes[i]; // make stream from array of bytes
    return std::istringstream(ss.str());
}

void testRead(const unsigned char* bytes, int length) {
    auto stream = streamFromArray(bytes, length);
    HDRImage image(stream);

    assert(image._width == 3);
    assert(image._height == 2);

    assert(image.getPixel(0, 0).isClose(Color(1.0e1, 2.0e1, 3.0e1)));
    assert(image.getPixel(1, 0).isClose(Color(4.0e1, 5.0e1, 6.0e1)));
    assert(image.getPixel(2, 0).isClose(Color(7.0e1, 8.0e1, 9.0e1)));
    assert(image.getPixel(0, 1).isClose(Color(1.0e2, 2.0e2, 3.0e2)));
    assert(image.getPixel(1, 1).isClose(Color(4.0e2, 5.0e2, 6.0e2)));
    assert(image.getPixel(2, 1).isClose(Color(7.0e2, 8.0e2, 9.0e2)));
}

void testReadFile(std::string name) {
    HDRImage image(name);

    assert(image._width == 3);
    assert(image._height == 2);

    assert(image.getPixel(0, 0).isClose(Color(1.0e1, 2.0e1, 3.0e1)));
    assert(image.getPixel(1, 0).isClose(Color(4.0e1, 5.0e1, 6.0e1)));
    assert(image.getPixel(2, 0).isClose(Color(7.0e1, 8.0e1, 9.0e1)));
    assert(image.getPixel(0, 1).isClose(Color(1.0e2, 2.0e2, 3.0e2)));
    assert(image.getPixel(1, 1).isClose(Color(4.0e2, 5.0e2, 6.0e2)));
    assert(image.getPixel(2, 1).isClose(Color(7.0e2, 8.0e2, 9.0e2)));
}

void testWritePMF() {
    auto stream = streamFromArray(LE_REFERENCE_BYTES, 84);
    HDRImage image(stream);
    image.save("../RayTracer/test/test.pfm");
    testReadFile("../RayTracer/test/test.pfm");
}

bool isClose(float a, float b) {
    return std::fabs(a - b) < 1e-6;
}

// tests if an exception is thrown by a function with the specified parameter
// if the function accepts more parameters, or if it is a constructor, use a lambda: 
// testException(parameter, [](Parameter p) -> {return function(p, [...]);});
template<typename Parameter, typename Function>
void testException(Parameter& parameter, Function function) {
    bool exceptionThrown = false;
    try{function(parameter);}
    catch(std::exception& e){exceptionThrown = true;}
    assert(exceptionThrown);
}



int main(){
    bool exceptionThrown = false; // used to test exception, reset to false after each test

    // test readLine
    std::istringstream iss("hiii\nthis is a test!");
    assert(readLine(iss) == "hiii");
    assert(readLine(iss) == "this is a test!");
    testException(iss, readLine);
    cout << "readLine works" << endl;

    // test parseImageSize
    assert(parseImageSize("2 5") == std::make_pair(2, 5));
    testException("-2 5", parseImageSize); // negative size
    testException("2 0", parseImageSize); // zero size
    testException("2 a", parseImageSize); // not int
    testException("2", parseImageSize); // string too short
    testException("2 5 3", parseImageSize); // string too long
    cout << "parseImageSize works" << endl;

    // test parseEndianness
    assert(parseEndianness("-1.0") == Endianness::LITTLE);
    assert(parseEndianness("1000.0") == Endianness::BIG);
    testException("e", parseEndianness); // can't convert to float
    testException("0.0", parseEndianness); // is zero
    cout << "parseEndianness works" << endl;

    // test readFloat
    unsigned char toRead[] = {0x00, 0x00, 0xc8, 0x42, 0x43, 0x48, 0x00, 0x00, 0x00}; // {100 (le), 200 (be), bonus byte}
    auto stream = streamFromArray(toRead, 9);
    assert(isClose(readFloat(stream, Endianness::LITTLE), 100));
    assert(isClose(readFloat(stream, Endianness::BIG), 200));
    testException(stream, [](std::istringstream& ss) -> auto {return readFloat(ss, Endianness::BIG);}); // only 1 byte left
    cout << "readFloat works" << endl;

    // test reading images
    testRead(LE_REFERENCE_BYTES, 84);
    testRead(BE_REFERENCE_BYTES, 83);
    std::istringstream ss("PF\n3 2\n-1.0\nstop");
    testException(ss, [](std::istringstream& s) -> auto {return HDRImage(s);});
    cout << "readPFM works" << endl;

    testReadFile("../RayTracer/test/reference_le.pfm");
    testReadFile("../RayTracer/test/reference_be.pfm");
    HDRImage image("../RayTracer/test/memorial.pfm");
    testException("../RayTracer/test/iDoNotExist.psd", testReadFile);
    cout << "readPFM from file works" << endl;

    // test writePFM
    testWritePMF();
    cout << "writePFM works" << endl;

    // test writing LDR formats
    stream = streamFromArray(TEST_WRITE_BYTES, 84);
    image = HDRImage(stream);
    image.normalize(1., image.averageLuminosity());
    image.clamp();
    image.save("../RayTracer/test/test.png");
    image.save("../RayTracer/test/test.jpeg");
    testException("../RayTracer/test/test.gif", [&image](std::string s) -> auto {return image.save(s);});
    cout << "there should be \"test.*\" files containing 6 pixels: g, w, r / y, m, k" << endl;



    /* // test readPFM
    unsigned char small[] = {0x50, 0x46, 0x0a, 0x31, 0x20, 0x31, 0x0a, 0x2d, 0x31, 0x2e, 0x30, 0x0a,
                             0x00, 0x00, 0xc8, 0x42, 0x00, 0x00, 0x48, 0x43, 0x00, 0x00, 0x96, 0x43};
    std::stringstream ss2;
    for (int i = 0; i < 24; i++) ss2 << small[i]; // make stream from array of bytes
    std::istringstream stream2(ss2.str());
    //cout << ss2.str() << endl;
    std::string s;
    stream2 >> s; // PF
    cout << s << endl;
    stream2 >> s; // width
    cout << s << endl;
    stream2 >> s; // height
    cout << s << endl;
    stream2 >> s; // endianness
    cout << s << endl;
    
    cout << readFloat(stream2, Endianness::LITTLE) << endl;
    cout << readFloat(stream2, Endianness::LITTLE) << endl;
    cout << readFloat(stream2, Endianness::LITTLE) << endl; */
    
    /* HDRImage image(stream2);
    cout << image._width << image._height << endl;
    cout << image.getPixel(0, 0).r << image.getPixel(0, 0).g << image.getPixel(0, 0).b << endl; */

    return 0;
}