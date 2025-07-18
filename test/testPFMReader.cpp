#include "HDRImage.hpp"
#include "utils.hpp"

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

    sassert(image._width == 3);
    sassert(image._height == 2);

    sassert(image.getPixel(0, 0).isClose(Color(1.0e1, 2.0e1, 3.0e1)));
    sassert(image.getPixel(1, 0).isClose(Color(4.0e1, 5.0e1, 6.0e1)));
    sassert(image.getPixel(2, 0).isClose(Color(7.0e1, 8.0e1, 9.0e1)));
    sassert(image.getPixel(0, 1).isClose(Color(1.0e2, 2.0e2, 3.0e2)));
    sassert(image.getPixel(1, 1).isClose(Color(4.0e2, 5.0e2, 6.0e2)));
    sassert(image.getPixel(2, 1).isClose(Color(7.0e2, 8.0e2, 9.0e2)));
}

void testReadFile(std::string name) {
    HDRImage image(name);

    sassert(image._width == 3);
    sassert(image._height == 2);

    sassert(image.getPixel(0, 0).isClose(Color(1.0e1, 2.0e1, 3.0e1)));
    sassert(image.getPixel(1, 0).isClose(Color(4.0e1, 5.0e1, 6.0e1)));
    sassert(image.getPixel(2, 0).isClose(Color(7.0e1, 8.0e1, 9.0e1)));
    sassert(image.getPixel(0, 1).isClose(Color(1.0e2, 2.0e2, 3.0e2)));
    sassert(image.getPixel(1, 1).isClose(Color(4.0e2, 5.0e2, 6.0e2)));
    sassert(image.getPixel(2, 1).isClose(Color(7.0e2, 8.0e2, 9.0e2)));
}

void testWritePMF() {
    auto stream = streamFromArray(LE_REFERENCE_BYTES, 84);
    HDRImage image(stream);
    image.save("../test/test.pfm");
    testReadFile("../test/test.pfm");
}



int main(){
    bool exceptionThrown = false; // used to test exception, reset to false after each test

    // test readLine
    std::istringstream iss("hiii\nthis is a test!");
    sassert(readLine(iss) == "hiii");
    sassert(readLine(iss) == "this is a test!");
    testException(iss, readLine);
    cout << "readLine works" << endl;

    // test parseImageSize
    sassert(parseImageSize("2 5") == std::make_pair(2, 5));
    testException("-2 5", parseImageSize); // negative size
    testException("2 0", parseImageSize); // zero size
    testException("2 a", parseImageSize); // not int
    testException("2", parseImageSize); // string too short
    testException("2 5 3", parseImageSize); // string too long
    cout << "parseImageSize works" << endl;

    // test parseEndianness
    sassert(parseEndianness("-1.0") == Endianness::LITTLE);
    sassert(parseEndianness("1000.0") == Endianness::BIG);
    testException("e", parseEndianness); // can't convert to float
    testException("0.0", parseEndianness); // is zero
    cout << "parseEndianness works" << endl;

    // test readFloat
    unsigned char toRead[] = {0x00, 0x00, 0xc8, 0x42, 0x43, 0x48, 0x00, 0x00, 0x00}; // {100 (le), 200 (be), bonus byte}
    auto stream = streamFromArray(toRead, 9);
    sassert(areClose(readFloat(stream, Endianness::LITTLE), 100));
    sassert(areClose(readFloat(stream, Endianness::BIG), 200));
    testException(stream, [](std::istringstream& ss) -> auto {return readFloat(ss, Endianness::BIG);}); // only 1 byte left
    cout << "readFloat works" << endl;

    // test reading images
    testRead(LE_REFERENCE_BYTES, 84);
    testRead(BE_REFERENCE_BYTES, 83);
    std::istringstream ss("PF\n3 2\n-1.0\nstop");
    testException(ss, [](std::istringstream& s) -> auto {return HDRImage(s);});
    cout << "readPFM works" << endl;

    testReadFile("../test/reference_le.pfm");
    testReadFile("../test/reference_be.pfm");
    HDRImage image("../test/memorial.pfm");
    testException("../test/iDoNotExist.psd", testReadFile);
    cout << "readPFM from file works" << endl;

    // test writePFM
    testWritePMF();
    cout << "writePFM works" << endl;

    // test writing LDR formats
    stream = streamFromArray(TEST_WRITE_BYTES, 84);
    image = HDRImage(stream);
    image.normalize(1., image.averageLuminosity());
    image.clamp();
    image.save("../test/test.png");
    image.save("../test/test.jpeg");
    testException("../test/test.gif", [&image](std::string s) -> auto {return image.save(s);});
    cout << "there should be \"test.*\" files containing 6 pixels: g, w, r / y, m, k" << endl;

    return 0;
}
