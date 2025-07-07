#include <iostream>
#include "../utils.hpp"
#include "../scenefile.hpp"

using std::cout, std::endl;

void testFileRegistry() {
    // test initialization
    FileRegistry::registerFile("txt.txt");
    sassert(FileRegistry::getFile(0) == "txt.txt");

    // test register
    int in = FileRegistry::registerFile("in.in");
    FileRegistry::registerFile("out.out");
    int config = FileRegistry::registerFile("config.config");
    sassert(FileRegistry::getFile(2) == "out.out");
    sassert(config == 3);

    // test duplicate file
    sassert(FileRegistry::registerFile("in.in") == in);

    cout << "FileRegistry works" << endl;
}

void testInputStream() {
    std::istringstream ss;
    ss.str("abc   \nd\nef");
    InputStream stream(ss, 0);

    sassert(stream._location.line == 1);
    sassert(stream._location.column == 1);

    sassert(stream.read() == 'a');
    sassert(stream._location.line == 1);
    sassert(stream._location.column == 2);

    stream.unread('A');
    sassert(stream._location.line == 1);
    sassert(stream._location.column == 1);

    sassert(stream.read() == 'A');
    sassert(stream._location.line == 1);
    sassert(stream._location.column == 2);

    sassert(stream.read() == 'b');
    sassert(stream._location.line == 1);
    sassert(stream._location.column == 3);

    sassert(stream.read() == 'c');
    sassert(stream._location.line == 1);
    sassert(stream._location.column == 4);

    stream._skipWsAndComments();

    // test peek
    sassert(stream.peek() == 'd'); // 'd' is altready the _savedChar, being unread in _skipWsAndComments
    sassert(stream.peek() == 'd'); // still shouldn't raise an error
    stream.unread('d');            // put back a charactar we didn't pull out
    sassert(stream.peek() == 'd');
    sassert(stream.read() == 'd'); // should read the peeked char
    stream.unread('d');

    sassert(stream.read() == 'd');
    sassert(stream._location.line == 2);
    sassert(stream._location.column == 2);

    sassert(stream.read() == '\n');
    sassert(stream._location.line == 3);
    sassert(stream._location.column == 1);

    sassert(stream.read() == 'e');
    sassert(stream._location.line == 3);
    sassert(stream._location.column == 2);

    sassert(stream.read() == 'f');
    sassert(stream._location.line == 3);
    sassert(stream._location.column == 3);

    sassert(stream.read() == '\0'); // a std::string ends with \0

    cout << "InputStream works" << endl;
}

bool isKeyword(Token t, Keywords tag) {
    return t.value.keyword == tag;
}

bool isIdentifier(Token t, const std::string& str) {
    return t.value.string == str;
}

bool isSymbol(Token t, const char& c) {
    return t.value.symbol == c;
}

bool isString(Token t, std::string str) {
    return t.value.string == str;
}

void testLexer() {
    std::istringstream ss;
    ss.str(
        "# This is a comment\n"
        "# This is another comment\n"
        "new material sky_material(\n"
        "    diffuse(image(\"my file.pfm\")),\n"
        "    <5.0, 500.0, 300.0>\n"
        ") # Comment at the end of the line"
    );
    InputStream stream(ss, 0);
    
    sassert(isKeyword(stream.readToken(), Keywords::NEW));
    sassert(isKeyword(stream.readToken(), Keywords::MATERIAL));
    sassert(isIdentifier(stream.readToken(), "sky_material"));
    sassert(isSymbol(stream.readToken(), '('));
    sassert(isKeyword(stream.readToken(), Keywords::DIFFUSE));
    sassert(isSymbol(stream.readToken(), '('));
    sassert(isKeyword(stream.readToken(), Keywords::IMAGE));
    sassert(isSymbol(stream.readToken(), '('));
    sassert(isString(stream.readToken(), "my file.pfm"));
    sassert(isSymbol(stream.readToken(), ')'));

    cout << "the lexer works" << endl;
}

void testThrowGrammarError() {
    std::istringstream ss;
    ss.str("new $ new      / n?ew");
    InputStream stream(ss, 0);

    sassert(isKeyword(stream.readToken(), Keywords::NEW)); // new (keyword)
    testException(stream, [](InputStream& s) {s.readToken();}); // $
    sassert(isKeyword(stream.readToken(), Keywords::NEW)); // new
    testException(stream, [](InputStream& s) {s.readToken();}); // /
    sassert(isIdentifier(stream.readToken(), "n")); // n (identifier)
    testException(stream, [](InputStream& s) {s.readToken();}); // ?
    sassert(isIdentifier(stream.readToken(), "ew")); // ew (identifier)

    std::istringstream ss2, ss3, ss4; // multiple streams
    ss2.str("1.2.3");                 // since we interrupt
    ss3.str("7e8888888");             // reading before the
    ss4.str("1.e3E2");                // token ends
    InputStream stream2(ss2, 0), stream3(ss3, 0), stream4(ss4, 0);

    testException(stream2, [](InputStream& s) {s.readToken();}); // two dots
    testException(stream3, [](InputStream& s) {s.readToken();}); // too big
    testException(stream4, [](InputStream& s) {s.readToken();}); // two exponentials

    cout << "grammar errors are handled correctly" << endl;
}

void testParser() {
    std::istringstream ss;
    ss.str(
        "float clock(150)\n\n"

        "material sky_material(\n"
        "    diffuse(\n"
        "        uniform(<0, 0, 0>),\n"
        "        uniform(<0.7, 0.5, 1>)\n"
        "    )\n"
        ")\n\n"

        "# Here is a comment\n\n"

        "material ground_material(\n"
        "    diffuse(checkered(<0.3, 0.5, 0.1>,\n"
        "                        <0.1, 0.2, 0.5>, 4),\n"
        "    uniform(<0, 0, 0>))\n"
        ")\n\n"

        "material sphere_material(\n"
        "    specular(uniform(<0.5, 0.5, 0.5>),\n"
        "    uniform(<0, 0, 0>))\n"
        ")\n\n"

        "plane (sky_material, translation([0, 0, 100]) * rotationY(clock))\n"
        "plane (ground_material, identity)\n\n"

        "sphere(sphere_material, translation([0, 0, 1]))\n\n"

        "camera(perspective, 1.0, 100, 2.0, rotationZ(30) * translation([-4, 0, 1]))\n\n"

        "pointLight([1., 1., 1.], <0., 0.1, 4>, 2)"
    );
    InputStream stream(ss, "testfile.fake");

    Scene scene;
    scene.parse(stream);

    // float variables
    sassert(scene.floatVariables.size() == 1);
    sassert(scene.floatVariables.find("clock") != scene.floatVariables.end());
    sassert(scene.floatVariables["clock"] == 150.);

    // materials
    sassert(scene.materials.size() == 3);
    sassert(scene.materials.find("sphere_material") != scene.materials.end());
    sassert(scene.materials.find("sky_material") != scene.materials.end());
    sassert(scene.materials.find("ground_material") != scene.materials.end());

    auto sphereMaterial = scene.materials["sphere_material"];
    auto skyMaterial = scene.materials["sky_material"];
    auto groundMaterial = scene.materials["ground_material"];

    sassert(skyMaterial->color({0., 0.}).isClose(Color(0., 0., 0.)));

    sassert(groundMaterial->color({0., 0.}).isClose(Color(0.3, 0.5, 0.1)));
    sassert(groundMaterial->color({0.2501, 0.}).isClose(Color(0.1, 0.2, 0.5))); // 0.2501 instead of 0.25 just to be sure...

    sassert(sphereMaterial->color({0., 0.}).isClose(Color(0.5, 0.5, 0.5)));

    sassert(skyMaterial->emittedColor({0., 0.}).isClose(Color(0.7, 0.5, 1.)));
    sassert(groundMaterial->emittedColor({0., 0.}).isClose(Color(0., 0., 0.)));
    sassert(sphereMaterial->emittedColor({0., 0.}).isClose(Color(0, 0, 0)));

    // shapes
    sassert(scene.world._shapes.size() == 3);
    sassert(scene.world._shapes[0]->transformation.isClose(translation(Vec3(0., 0., 100.)) * rotation(150., Axis::Y)));
    sassert(scene.world._shapes[1]->transformation.isClose(Transformation()));
    sassert(scene.world._shapes[2]->transformation.isClose(translation(Vec3(0., 0., 1.))));

    // camera
    sassert(scene.camera->transformation.isClose(rotation(30., Axis::Z) * translation(Vec3(-4., 0., 1.))));
    sassert(areClose(scene.camera->aspectRatio, 1.));
    sassert(areClose((float) scene.camera->imageWidth, 100.));

    // pointLight
    PointLight p = scene.world.pointLights[0];
    sassert(p.color.isClose(Color(0., 0.1, 4.)));
    sassert(p.position.isClose(Point3(1., 1., 1.)));
    sassert(areClose(p.linearRadius, 2.));

    cout << "parser works" << endl;
}

void testUndefinedMaterial() {
    std::istringstream ss;
    ss.str("plane(this_material_does_not_exist, identity");
    InputStream stream(ss, 0);

    testException(stream, [](InputStream s){ Scene scene; scene.parse(s); });

    cout << "undefined materials are handled correctly" << endl;
}

void testDoubleCamera() {
    std::istringstream ss;
    ss.str(
        "camera(perspective, rotationZ(30) * translation([-4, 0, 1]), 1.0, 1.0)\n"
        "camera(orthogonal, identity, 1.0, 1.0)"
    );
    InputStream stream(ss, 0);

    testException(stream, [](InputStream s){ Scene scene; scene.parse(s); });

    cout << "second camera is handled correctly" << endl;
}



int main() {
    testFileRegistry();
    testInputStream();
    testLexer();
    testThrowGrammarError();

    testParser();
    testUndefinedMaterial();
    testDoubleCamera();

    return 0;
}