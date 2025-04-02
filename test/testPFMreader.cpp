#include <cassert>
#include "../HDRImage.hpp"

using std::cout, std::endl;

// This is the content of "reference_le.pfm" (little-endian file)
unsigned char LE_REFERENCE_BYTES[] = {
    0x50, 0x46, 0x0a, 0x33, 0x20, 0x32, 0x0a, 0x2d, 0x31, 0x2e, 0x30, 0x0a,
    0x00, 0x00, 0xc8, 0x42, 0x00, 0x00, 0x48, 0x43, 0x00, 0x00, 0x96, 0x43,
    0x00, 0x00, 0xc8, 0x43, 0x00, 0x00, 0xfa, 0x43, 0x00, 0x00, 0x16, 0x44,
    0x00, 0x00, 0x2f, 0x44, 0x00, 0x00, 0x48, 0x44, 0x00, 0x00, 0x61, 0x44,
    0x00, 0x00, 0x20, 0x41, 0x00, 0x00, 0xa0, 0x41, 0x00, 0x00, 0xf0, 0x41,
    0x00, 0x00, 0x20, 0x42, 0x00, 0x00, 0x48, 0x42, 0x00, 0x00, 0x70, 0x42,
    0x00, 0x00, 0x8c, 0x42, 0x00, 0x00, 0xa0, 0x42, 0x00, 0x00, 0xb4, 0x42
};

// This is the content of "reference_be.pfm" (big-endian file)
unsigned char BE_REFERENCE_BYTES[] = {
    0x50, 0x46, 0x0a, 0x33, 0x20, 0x32, 0x0a, 0x31, 0x2e, 0x30, 0x0a,
    0x42, 0xc8, 0x00, 0x00, 0x43, 0x48, 0x00, 0x00, 0x43, 0x96, 0x00, 0x00,
    0x43, 0xc8, 0x00, 0x00, 0x43, 0xfa, 0x00, 0x00, 0x44, 0x16, 0x00, 0x00,
    0x44, 0x2f, 0x00, 0x00, 0x44, 0x48, 0x00, 0x00, 0x44, 0x61, 0x00, 0x00,
    0x41, 0x20, 0x00, 0x00, 0x41, 0xa0, 0x00, 0x00, 0x41, 0xf0, 0x00, 0x00,
    0x42, 0x20, 0x00, 0x00, 0x42, 0x48, 0x00, 0x00, 0x42, 0x70, 0x00, 0x00,
    0x42, 0x8c, 0x00, 0x00, 0x42, 0xa0, 0x00, 0x00, 0x42, 0xb4, 0x00, 0x00
};

void testRead(unsigned char* bytes, int length) {
    std::stringstream ss;
    for (int i = 0; i < length; i++) ss << bytes[i]; // make stream from array of bytes
    std::istringstream stream(ss.str());
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

void testReadFile(std::string name, int length) {
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

bool isClose(float a, float b) {
    return std::fabs(a - b) < 1e-6;
}

/* def test_pfm_read_wrong(self):
    buf = BytesIO(b"PF\n3 2\n-1.0\nstop")
    with pytest.raises(InvalidPfmFileFormat):
        _ = read_pfm_image(buf) */

int main(){
    // test readLine
    std::istringstream iss("hiii\nthis is a test!");
    assert(readLine(iss) == "hiii");
    assert(readLine(iss) == "this is a test!");
    //std::string buffer = readLine(iss); // this throws an error
    cout << "readLine works" << endl;

    // test parseImageSize
    assert(parseImageSize("2 5") == std::make_pair(2, 5));
    //auto size1 = parseImageSize("-2 5"); // negative size
    //auto size2 = parseImageSize("2 0"); // zero size
    //auto size3 = parseImageSize("2 a"); // not int
    //auto size4 = parseImageSize("2"); // string too short
    //auto size5 = parseImageSize("2 2 a"); // string too long
    cout << "parseImageSize works" << endl;

    // test parseEndianness
    assert(parseEndianness("-1.0") == Endianness::LITTLE);
    assert(parseEndianness("1000.0") == Endianness::BIG);
    //auto endianness1 = parseEndianness("endianness"); // can't convert to float
    //auto endianness2 = parseEndianness("0.0"); // is zero
    cout << "parseEndianness works" << endl;

    // test readFloat
    unsigned char toRead[] = {0x00, 0x00, 0xc8, 0x42, 0x43, 0x48, 0x00, 0x00}; // {100 (le), 200 (be)}
    std::stringstream ss;
    for (int i = 0; i < 8; i++) ss << toRead[i]; // make stream from array of bytes
    std::istringstream stream(ss.str());
    
    assert(isClose(readFloat(stream, Endianness::LITTLE), 100));
    assert(isClose(readFloat(stream, Endianness::BIG), 200));
    cout << "readFloat works" << endl;

    testRead(LE_REFERENCE_BYTES, 84);
    testRead(BE_REFERENCE_BYTES, 83);
    cout << "readPFM works" << endl;

    testReadFile("test/reference_le.pfm", 84);
    testReadFile("test/reference_be.pfm", 83);
    cout << "readPFM from file works" << endl;

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