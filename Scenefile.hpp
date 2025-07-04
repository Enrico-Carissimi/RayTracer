#ifndef __SceneFile__
#define __SceneFile__

#include <string>
#include <vector>
#include <exception>
#include <sstream>
#include <optional>
#include <unordered_map>
#include <memory>
#include <set>

#include "utils.hpp"
#include "World.hpp"
#include "Vec3.hpp"
#include "Transformation.hpp"
#include "shapes.hpp"
#include "materials.hpp"
#include "Camera.hpp"
#include "Color.hpp"

struct FileRegistry {
public:
    static int registerFile(const std::string& filename) {
        for (size_t i = 0; i < _files.size(); ++i) {
            if (_files[i] == filename) return static_cast<int>(i);
        }
        _files.push_back(filename);
        return static_cast<int>(_files.size() - 1);
    }

    static const std::string& getFile(int index) {
        return _files.at(index);
    }

private:
    static std::vector<std::string> _files;
};

inline std::vector<std::string> FileRegistry::_files = {};



struct SourceLocation {
public:
    int line;
    int column;

    SourceLocation(int fileIndex = -1, int line = 0, int col = 0)
        : line(line), column(col), _fileIndex(fileIndex) {}

    std::string fileName() const {
        return _fileIndex >= 0 ? FileRegistry::getFile(_fileIndex) : "";
    }

private:
    int _fileIndex; // index into FileRegistry
};



const std::string SYMBOLS = ",()[]<>*";

enum class Keywords {
    NEW,
    FLOAT,
    IDENTITY, // transformations
    TRANSLATION,
    ROTATION_X, ROTATION_Y, ROTATION_Z,
    SCALING,
    CAMERA, ORTHOGONAL, PERSPECTIVE, // cameras
    SPHERE, PLANE, POINT_LIGHT, // shapes
    MATERIAL,
    UNIFORM, CHECKERED, IMAGE, // textures
    DIFFUSE, SPECULAR // materials
};

const std::unordered_map<std::string, Keywords> KEYWORDS {
    {"new", Keywords::NEW},
    {"float", Keywords::FLOAT},
    {"identity", Keywords::IDENTITY},
    {"translation", Keywords::TRANSLATION},
    {"rotationX", Keywords::ROTATION_X},
    {"rotationY", Keywords::ROTATION_Y},
    {"rotationZ", Keywords::ROTATION_Z},
    {"scaling", Keywords::SCALING},
    {"camera", Keywords::CAMERA},
    {"orthogonal", Keywords::ORTHOGONAL},
    {"perspective", Keywords::PERSPECTIVE},
    {"sphere", Keywords::SPHERE},
    {"plane", Keywords::PLANE},
    {"pointLight", Keywords::POINT_LIGHT},
    {"material", Keywords::MATERIAL},
    {"uniform", Keywords::UNIFORM},
    {"checkered", Keywords::CHECKERED},
    {"image", Keywords::IMAGE},
    {"diffuse", Keywords::DIFFUSE},
    {"specular", Keywords::SPECULAR}
};

// from https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
struct _EnumClassHash {     // we need a custom hash functin to use
    template <typename T>   // user defined data as map keys
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

const std::unordered_map<Keywords, std::string, _EnumClassHash> INVERSE_KEYWORDS {
    {Keywords::NEW, "new"},
    {Keywords::FLOAT, "float"},
    {Keywords::IDENTITY, "identity"},
    {Keywords::TRANSLATION, "translation"},   
    {Keywords::ROTATION_X, "rotationX"},
    {Keywords::ROTATION_Y, "rotationY"},
    {Keywords::ROTATION_Z, "rotationZ"},
    {Keywords::SCALING, "scaling"},
    {Keywords::CAMERA, "camera"},
    {Keywords::ORTHOGONAL, "orthogonal"},
    {Keywords::PERSPECTIVE, "perspective"},
    {Keywords::SPHERE, "sphere"},
    {Keywords::PLANE, "plane"},
    {Keywords::POINT_LIGHT, "pointLight"},
    {Keywords::MATERIAL, "material"},
    {Keywords::UNIFORM, "uniform"},
    {Keywords::CHECKERED, "checkered"},
    {Keywords::IMAGE, "image"},
    {Keywords::DIFFUSE, "diffuse"},
    {Keywords::SPECULAR, "specular"}
};

enum class TokenTags {
    KEYWORD, IDENTIFIER, STRING_LITERAL, NUMBER_LITERAL, SYMBOL, STOP
};

union TokenUnion {
    Keywords keyword;
    std::string string; // string literal and identifier
    float numberLiteral;
    char symbol;

    TokenUnion() : numberLiteral(0) {}
    TokenUnion(Keywords k) : keyword(k) {}
    TokenUnion(const std::string& s) : string(s) {}
    TokenUnion(float n) : numberLiteral(n) {}
    TokenUnion(const char& c) : symbol(c) {}
    ~TokenUnion() {}
};

struct Token {
    TokenTags tag;
    TokenUnion value;
    SourceLocation location;

    Token(TokenTags tag, Keywords k, SourceLocation location)
        : tag(tag), value(k), location(location) {}

    Token(TokenTags tag, const std::string& string, SourceLocation location)
        : tag(tag), value(string), location(location) {}

    Token(TokenTags tag, float numberLiteral, SourceLocation location)
        : tag(tag), value(numberLiteral), location(location) {}

    Token(TokenTags tag, const char& symbol, SourceLocation location)
        : tag(tag), value(symbol), location(location) {}

    Token(const Token& other) {*this = other;}

    Token& operator=(const Token& other) {
        tag = other.tag;
        location = other.location;

        switch (tag) {
            case TokenTags::KEYWORD:
                value.keyword = other.value.keyword;
                break;
            case TokenTags::IDENTIFIER:
                value.string = other.value.string;
                break;
            case TokenTags::STRING_LITERAL:
                value.string = other.value.string;
                break;
            case TokenTags::NUMBER_LITERAL:
                value.numberLiteral = other.value.numberLiteral;
                break;
            case TokenTags::SYMBOL:
                value.symbol = other.value.symbol;
                break;
            case TokenTags::STOP:
                value.symbol = other.value.symbol;
                break;
            }
        return *this;
    }

    ~Token() {
        if (tag == TokenTags::IDENTIFIER || tag == TokenTags::STRING_LITERAL)
            value.string.~basic_string();
    }

    std::string toString() {
        switch (tag) {
            case TokenTags::KEYWORD:
                return "<keyword>";
            case TokenTags::IDENTIFIER:
                return "\"" + value.string + "\"";
            case TokenTags::STRING_LITERAL:
                return "\"" + value.string + "\"";
            case TokenTags::NUMBER_LITERAL:
                return std::to_string(value.numberLiteral);
            case TokenTags::SYMBOL:
                return "'" + std::string(1, value.symbol) + "'";
            case TokenTags::STOP:
                return "<EOF>";
            default:
                return "<unknown>";
        }
    }
};



class GrammarError : public std::exception {
public:
    GrammarError(const SourceLocation& location, const std::string& message)
        : _location(location), _message(message) {
        std::ostringstream oss;
        oss << _location.fileName() << ":" << _location.line << ":" << _location.column
            << ": " << _message;
        _fullMessage = oss.str();
    }

    const char* what() const noexcept override {
        return _fullMessage.c_str();
    }

private:
    SourceLocation _location;
    std::string _message;
    std::string _fullMessage;
};



class InputStream {
public:
    InputStream(std::istream& stream, int fileIndex, int tabs = 4) // I'm writing in vscode with tab = 4 spaces
        : _location(fileIndex, 1, 1), _stream(stream), _tabs(tabs), _savedLocation() {}
    InputStream(std::istream& stream, const std::string& fileName, int tabs = 4)
        : _location(FileRegistry::registerFile(fileName), 1, 1), _stream(stream), _tabs(tabs), _savedLocation() {}

    /**
     * @brief Reads a character from the stream
     * 
     * @return char 
     */
    char read();

    /**
     * @brief Puts the last character read back in the stream
     * 
     * @param c the character to put back
     */
    void unread(const char& c);

    /**
     * @brief Looks ahead one character without extracting it from the stream
     * 
     * @return char
     */
    char peek() {
        char c = read(); // sets _peeking to false
        _peeking = true;
        unread(c);
        return c;
    }

    /**
     * @brief Reads a token from the stream (keyword, variable identifier, string or number literal, symbol, or EOF)
     * 
     * @return Token 
    */
    Token readToken ();

    /**
     * @brief Puts the last token read back in the stream
     * 
     * @param token the token to put back
     */
    void unreadToken(const Token& token) {
        if (_savedToken.has_value()) {
            std::cout << "ERROR: there's already an unread token" << std::endl;
            exit(-1);
        }
        _savedToken = token;
    }
    


    SourceLocation _location;

    void _skipWsAndComments();

private:
    std::istream& _stream;
    int _tabs;
    std::optional<char> _savedChar;
    SourceLocation _savedLocation;
    bool _peeking = false;
    std::optional<Token> _savedToken;

    void updateLocation(const char& c) {
        if (c == '\0') return; // EOF, nothing to do

        if (c == '\n') _location.column = 1, _location.line++; // it would be more efficient to run the checks
        else if (c == '\t') _location.column += _tabs;         // in opposite order (normal char, tab, newline, EOF)
        else _location.column++;                               // but here we don't care about efficiency
    }

    void skipComment() {
        char c = '#';
        while (c != '\n' && c != '\r' && c != '\0') c = read();
    }

    Token readIdentifierOrKeyword(SourceLocation location);
    Token readStringToken(SourceLocation location);
    Token readNumberToken(SourceLocation location);
};

class Scene {
public:
    World world;
    std::shared_ptr<Camera> camera; // shared pointer just to check if it's initialized
    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
    std::unordered_map<std::string, float> floatVariables;
    std::set<std::string> overriddenVariables; // easier to search than a vector

    Scene() {}
    Scene(std::string fileName, const std::unordered_map<std::string, float>& variables = std::unordered_map<std::string, float>()) {
        std::ifstream file(fileName);
        if (file.fail()) { //in the main we already check using CLI11, but you never know
            std::cout << "ERROR: impossible to open file \"" + fileName + "\"" << std::endl;
            exit(-1);
        }
        InputStream stream(file, fileName);
        parse(stream, variables);
    }

    void parse(InputStream& inputFile, const std::unordered_map<std::string, float>& variables = std::unordered_map<std::string, float>());

private:
    void expectSymbol(InputStream& inputFile, const char& symbol);
    Keywords expectKeywords(InputStream& inputFile, const std::vector<Keywords>& keywords);
    float expectNumber(InputStream& inputFile);
    std::string expectString(InputStream& inputFile);
    std::string expectIdentifier(InputStream& inputFile);

    Vec3 parseVector(InputStream& inputFile);
    Color parseColor(InputStream& inputFile);
    std::shared_ptr<Texture> parseTexture(InputStream& inputFile);
    void parseMaterial(InputStream& inputFile); // directly add the material to the map
    Transformation parseTransformation(InputStream& inputFile);
    void parseSphere(InputStream& inputFile);   // these functions directly modify world
    void parsePlane(InputStream& inputFile);
    void parsePointLight(InputStream& inputFile);
    void parseCamera(InputStream& inputFile);   // directly assign camera
};




// The following will be moved to a cpp file in a dedicated PR with other libraries



// InputStream

char InputStream::read() {
    char c;
    if (_savedChar.has_value()) {
        c = _savedChar.value();
        _savedChar.reset();
    } else {
        _stream.get(c); // we could skip whitespaces here
        if (_stream.eof()) c = '\0'; // cross-platform support I guess? Not needed on windows with g++
    }
    
    _savedLocation = _location;
    updateLocation(c);
    _peeking = false;

    return c;
}

void InputStream::unread(const char& c) {
    if (_savedChar.has_value() && !_peeking) {
        std::cout << "ERROR: there's already an unread character" << std::endl;
        exit(-1);
    }
    _savedChar = c; // we could use _stream.putback(c) and avoid using _savedChar
    _location = _savedLocation;
}

Token InputStream::readToken () {
    if (_savedToken.has_value()) {
        Token t = _savedToken.value();
        _savedToken.reset();
        return t;
    }

    _skipWsAndComments();
    char c = peek();

    if (c == '\0') return Token(TokenTags::STOP, '\0', _location); // EOF

    SourceLocation tokenLocation = _location;

    if (SYMBOLS.find(c) != std::string::npos) return Token(TokenTags::SYMBOL, read(), tokenLocation);
    else if (c == '"') {
        read(); // we need to skip the '"'
        return readStringToken(tokenLocation);
    }
    else if (isdigit(c) || c == '-') return readNumberToken(tokenLocation);
    else if (isalpha(c)) return readIdentifierOrKeyword(tokenLocation);
    else throw GrammarError(_location, "invalid character '" + std::string{read()} + "'");
}

void InputStream::_skipWsAndComments() {
    char c = read();
    while (isCharSkippable(c)) {
        if (c == '#') skipComment(); // skip to next line if there's a comment
        
        c = read();

        if (c == '\0') return;       // EOF, nothing to do
    }

    unread(c);                       // unread the last, non-skippable, char read
}

Token InputStream::readIdentifierOrKeyword(SourceLocation location) {
    std::string value = "";

    while (true) {
        if (!isalnum(peek()) && peek() != '_') break; // we know the first char is't a number
        value += read();
    }

    // if is not a keyword, return an identifier
    try { return Token(TokenTags::KEYWORD, KEYWORDS.at(value), location); } // at() throws exception if the key is not present
    catch (std::out_of_range& e) { return Token(TokenTags::IDENTIFIER, value, location); }
}

Token InputStream::readStringToken(SourceLocation location) {
    std::string value = "";

    while (true) {
        char c = read();
        if (c == '"') break; // end quote, we read a string after reading the starting quote
        if (c == '\0') throw GrammarError(location, "unterminated string");
        value += c;
    }

    return Token(TokenTags::STRING_LITERAL, value, location);
}

Token InputStream::readNumberToken(SourceLocation location) {
    std::string value = "";
    bool dots = false, es = false, minus = false;

    while (true) {
        char c = peek();
        if (!isdigit(c) && c != '.' && c != 'e' && c != 'E' && c != '-') break;

        // the following is needed because for example 1.2.3 is read correctly until the end,
        // and is then converted to 1.2 by stof without throwing errors
        if (c == '.')
            dots = dots ? throw GrammarError(_location, "too many '.' in float initialization") : true;

        // we should also check if there are multiple 'e' or 'E' characters, 1e2e3 becomes 100 otherwise
        else if (c == 'e' || c == 'E')
            es = es ? throw GrammarError(_location, "too many 'e's in float initialization") : true;

        // we should add similar checks for the '-' sign, since -1-2 is read but becomes just -1
        // but it's complicated by the (possible) exponential sign, and is probably useless anyway

        value += read();
    }

    float number;
    try { number = std::stof(value); }
    catch (std::out_of_range& e) { throw GrammarError(location, value + " is out of float range"); }
    catch (std::invalid_argument& e) { throw GrammarError(location, value + " is not a valid number"); }

    return Token(TokenTags::NUMBER_LITERAL, number, location);
}



// Scene

void Scene::expectSymbol(InputStream& inputFile, const char& symbol) {
    Token token = inputFile.readToken();
    if (token.tag != TokenTags::SYMBOL || token.value.symbol != symbol) {
        throw GrammarError(token.location, "expected '" + std::string{symbol} + "', got " + token.toString());
    }
}

Keywords Scene::expectKeywords(InputStream& inputFile, const std::vector<Keywords>& keywords) {
    Token token = inputFile.readToken();
    if (token.tag != TokenTags::KEYWORD) {
        throw GrammarError(token.location, "expected a keyword, got " + token.toString());
    }

    Keywords kw = token.value.keyword;
    if (std::find(keywords.begin(), keywords.end(), kw) == keywords.end()) {
        std::string expectedList;
        for (const auto& k : keywords)
            expectedList += INVERSE_KEYWORDS.at(k) + ", ";
        if (!expectedList.empty()) expectedList.pop_back(), expectedList.pop_back(); // remove last ", "
        throw GrammarError(token.location, "expected one of {" + expectedList + "}, got \"" + INVERSE_KEYWORDS.at(kw) + "\"");
    }

    return kw;
}

float Scene::expectNumber(InputStream& inputFile) {
    Token token = inputFile.readToken();

    if (token.tag == TokenTags::NUMBER_LITERAL) {
        return token.value.numberLiteral;
    } else if (token.tag == TokenTags::IDENTIFIER) {
        std::string name = token.value.string;
        if (!floatVariables.contains(name)) { // c++ 20 
            throw GrammarError(token.location, "unknown variable \"" + name + "\"");
        }
        return floatVariables[name];
    }

    throw GrammarError(token.location, "expected a number, got " + token.toString());
}

std::string Scene::expectString(InputStream& inputFile) {
    Token token = inputFile.readToken();
    if (token.tag != TokenTags::STRING_LITERAL) {
        throw GrammarError(token.location, "expected a string, got " + token.toString());
    }
    return token.value.string;
}

std::string Scene::expectIdentifier(InputStream& inputFile) {
    Token token = inputFile.readToken();
    if (token.tag != TokenTags::IDENTIFIER) {
        throw GrammarError(token.location, "expected an identifier, got " + token.toString());
    }
    return token.value.string;
}


Vec3 Scene::parseVector(InputStream& inputFile) {
    expectSymbol(inputFile, '[');
    float x = expectNumber(inputFile);
    expectSymbol(inputFile, ',');
    float y = expectNumber(inputFile);
    expectSymbol(inputFile, ',');
    float z = expectNumber(inputFile);
    expectSymbol(inputFile, ']');
    return Vec3(x, y, z);
}

Color Scene::parseColor(InputStream& inputFile) {
    expectSymbol(inputFile, '<');
    float r = expectNumber(inputFile);
    expectSymbol(inputFile, ',');
    float g = expectNumber(inputFile);
    expectSymbol(inputFile, ',');
    float b = expectNumber(inputFile);
    expectSymbol(inputFile, '>');
    return Color(r, g, b);
}

std::shared_ptr<Texture> Scene::parseTexture(InputStream& inputFile) {
    Keywords kw = expectKeywords(inputFile, {
        Keywords::UNIFORM, Keywords::CHECKERED, Keywords::IMAGE
    });

    expectSymbol(inputFile, '(');

    std::shared_ptr<Texture> result;

    if (kw == Keywords::UNIFORM) {
        Color color = Scene::parseColor(inputFile);
        result = std::make_shared<UniformTexture>(color);
    } else if (kw == Keywords::CHECKERED) {
        Color c1 = Scene::parseColor(inputFile);
        expectSymbol(inputFile, ',');
        Color c2 = Scene::parseColor(inputFile);
        expectSymbol(inputFile, ',');
        int steps = (int) expectNumber(inputFile);
        result = std::make_shared<CheckeredTexture>(c1, c2, steps);
    } else {
        std::string filename = expectString(inputFile);
        result = std::make_shared<ImageTexture>(HDRImage(filename));
    }

    expectSymbol(inputFile, ')');
    return result;
}

void Scene::parseMaterial(InputStream& inputFile) {
    std::string name = expectIdentifier(inputFile);
    expectSymbol(inputFile, '(');

    Keywords kw = expectKeywords(inputFile, {Keywords::DIFFUSE, Keywords::SPECULAR});
    
    expectSymbol(inputFile, '(');
    std::shared_ptr<Texture> texture = parseTexture(inputFile);
    expectSymbol(inputFile, ',');
    std::shared_ptr<Texture> emittedRadiance = parseTexture(inputFile);
    expectSymbol(inputFile, ')');

    expectSymbol(inputFile, ')');

    if (kw == Keywords::DIFFUSE)
        materials[name] = std::make_shared<DiffuseMaterial>(texture, emittedRadiance);
    else
        materials[name] = std::make_shared<SpecularMaterial>(texture, emittedRadiance);
}

Transformation Scene::parseTransformation(InputStream& inputFile) {
    Transformation result; // default is identity

    while (true) {
        Keywords kw = expectKeywords(inputFile, {
            Keywords::IDENTITY,
            Keywords::TRANSLATION,
            Keywords::ROTATION_X,
            Keywords::ROTATION_Y,
            Keywords::ROTATION_Z,
            Keywords::SCALING
        });

        if (kw != Keywords::IDENTITY) {
            expectSymbol(inputFile, '(');

            if (kw == Keywords::TRANSLATION) {
                result = result * translation(parseVector(inputFile));
            } else if (kw == Keywords::ROTATION_X) {
                result = result * rotation(expectNumber(inputFile), Axis::X);
            } else if (kw == Keywords::ROTATION_Y) {
                result = result * rotation(expectNumber(inputFile), Axis::Y);
            } else if (kw == Keywords::ROTATION_Z) {
                result = result * rotation(expectNumber(inputFile), Axis::Z);
            } else if (kw == Keywords::SCALING) {
                result = result * scaling(parseVector(inputFile));
            }

            expectSymbol(inputFile, ')');
        }

        Token t = inputFile.readToken();
        if (t.tag != TokenTags::SYMBOL || t.value.symbol != '*') {
            inputFile.unreadToken(t);
            break;
        }
    }

    return result;
}

void Scene::parseSphere(InputStream& inputFile) {
    expectSymbol(inputFile, '(');
    std::string material = expectIdentifier(inputFile);

    if (!materials.contains(material)) { // c++20
        throw GrammarError(inputFile._location, "unknown material: \"" + material + "\"");
    }

    expectSymbol(inputFile, ',');
    Transformation transf = parseTransformation(inputFile);
    expectSymbol(inputFile, ')');

    world.addShape(std::make_shared<Sphere>(materials[material], transf));
}

void Scene::parsePlane(InputStream& inputFile) {
    expectSymbol(inputFile, '(');
    std::string material = expectIdentifier(inputFile);

    if (!materials.contains(material)) { // c++20
        throw GrammarError(inputFile._location, "unknown material: \"" + material + "\"");
    }

    expectSymbol(inputFile, ',');
    Transformation transf = parseTransformation(inputFile);
    expectSymbol(inputFile, ')');

    world.addShape(std::make_shared<Plane>(materials[material], transf));
}

void Scene::parsePointLight(InputStream& inputFile) {
    expectSymbol(inputFile, '(');
    Vec3 position = parseVector(inputFile);
    expectSymbol(inputFile, ',');
    Color color = parseColor(inputFile);
    expectSymbol(inputFile, ',');
    float radius = expectNumber(inputFile);
    expectSymbol(inputFile, ')');

    world.addLight(PointLight(Point3(position.x, position.y, position.z), color, radius));
}

void Scene::parseCamera(InputStream& inputFile) {
    expectSymbol(inputFile, '(');
    Keywords kw = expectKeywords(inputFile, {Keywords::PERSPECTIVE, Keywords::ORTHOGONAL});
    expectSymbol(inputFile, ',');
    float aspectRatio = expectNumber(inputFile);
    expectSymbol(inputFile, ',');
    float imageWidth = expectNumber(inputFile);
    expectSymbol(inputFile, ',');
    float distance = expectNumber(inputFile);
    expectSymbol(inputFile, ',');
    Transformation transf = parseTransformation(inputFile);
    expectSymbol(inputFile, ')');

    if (kw == Keywords::PERSPECTIVE)
        camera = std::make_shared<Camera>("perspective", aspectRatio, imageWidth, distance, transf);
    else
        camera = std::make_shared<Camera>("orthogonal", aspectRatio, imageWidth, distance, transf);
}

void Scene::parse(InputStream& inputFile, const std::unordered_map<std::string, float>& variables) {
    floatVariables = variables;
    for (const auto& pair: variables) overriddenVariables.insert(pair.first);
    //overriddenVariables.insert(variables.begin(), variables.end());

    while (true) {
        Token token = inputFile.readToken();
        if (token.tag == TokenTags::STOP)
            break;

        if (token.tag != TokenTags::KEYWORD)
            throw GrammarError(token.location, "expected a keyword");

        std::string name;   // the compiler isn't happy even if
        SourceLocation loc; // these are used only in the FLOAT case
        float val;          // so we declare them before the switch
        switch (token.value.keyword) {
            case Keywords::FLOAT:
                name = expectIdentifier(inputFile);
                loc = inputFile._location;
                expectSymbol(inputFile, '(');
                val = expectNumber(inputFile);
                expectSymbol(inputFile, ')');

                if (floatVariables.contains(name) && !overriddenVariables.contains(name)) // c++20
                    throw GrammarError(loc, "redefinition of variable \"" + name + "\"");

                if (!overriddenVariables.contains(name)) // c++20
                    floatVariables[name] = val;

                break;
            case Keywords::SPHERE:
                parseSphere(inputFile);
                break;
            case Keywords::PLANE:
                parsePlane(inputFile);
                break;
            case Keywords::CAMERA:
                if (camera != nullptr)
                    throw GrammarError(token.location, "cannot define more than one camera");
                parseCamera(inputFile);
                break;
            case Keywords::MATERIAL: {
                parseMaterial(inputFile);
                break;
            }
            case Keywords::POINT_LIGHT: {
                parsePointLight(inputFile);
                break;
            }
            default:
                throw GrammarError(token.location, "unexpected keyword");
        }
    }
}

#endif