#ifndef __SceneFile__
#define __SceneFile__

#include <string>
#include <vector>
#include <exception>
#include <sstream>
#include <optional>
#include <unordered_map>
#include <map>
#include <memory>
#include <set>

#include "utils.hpp"
#include "World.hpp"
#include "Vec3.hpp"
#include "Transformation.hpp"
#include "shapes.hpp"
#include "renderers.hpp"
#include "materials.hpp"
#include "PFMreader.hpp"
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
        : _fileIndex(fileIndex), line(line), column(col) {}

    const std::string& fileName() const {
        return _fileIndex >= 0 ? FileRegistry::getFile(_fileIndex) : empty;
    }

private:
    int _fileIndex; // index into FileRegistry
    inline static const std::string empty = "";
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
    SPHERE, PLANE, // shapes
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
    {"material", Keywords::MATERIAL},
    {"uniform", Keywords::UNIFORM},
    {"checkered", Keywords::CHECKERED},
    {"image", Keywords::IMAGE},
    {"diffuse", Keywords::DIFFUSE},
    {"specular", Keywords::SPECULAR}
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
        :tag(tag), value(k), location(location) {}

    Token(TokenTags tag, const std::string& string, SourceLocation location)
        :tag(tag), value(string), location(location) {}

    Token(TokenTags tag, float numberLiteral, SourceLocation location)
        :tag(tag), value(numberLiteral), location(location) {}

    Token(TokenTags tag, const char& symbol, SourceLocation location)
        :tag(tag), value(symbol), location(location) {}

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
                new(&value.string) std::string(other.value.string);
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
        if (tag == TokenTags::IDENTIFIER || tag == TokenTags::STRING_LITERAL) {
            value.string.~basic_string();
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
        : _stream(stream), _location(fileIndex, 1, 1), _tabs(tabs), _savedLocation() {}
    InputStream(std::istream& stream, std::string fileName, int tabs = 4)
        : _stream(stream), _location(FileRegistry::registerFile(fileName), 1, 1), _tabs(tabs), _savedLocation() {}

    /**
     * @brief Reads a character from the stream
     * 
     * @return char 
     */
    char read() {
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

    /**
     * @brief Puts the last character read back in the stream
     * 
     * @param c the character to put back
     */
    void unread(const char& c) {
        if (_savedChar.has_value() && !_peeking) {
            std::cout << "ERROR: there's already an unread character" << std::endl;
            exit(-1);
        }
        _savedChar = c; // we could use _stream.putback(c) and avoid using _savedChar
        _location = _savedLocation;
    }

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
    Token readToken () {
        _skipWsAndComments();
        char c = peek();

        if (c == '\0') return Token(TokenTags::STOP, '\0', _location); // EOF

        SourceLocation tokenLocation = _location;

        if (SYMBOLS.find(c) != std::string::npos) return Token(TokenTags::SYMBOL, read(), tokenLocation);
        else if (c == '"') {
            read(); // we need to skip the '"'
            return readStringToken(tokenLocation);
        }
        else if (isdigit(c)) return readNumberToken(tokenLocation);
        else if (isalpha(c)) return readIdentifierOrKeyword(tokenLocation);
        else throw GrammarError(_location, "invalid character '" + std::string{read()} + "'");
    }
        

    

    SourceLocation _location;

    void _skipWsAndComments() {
        char c = read();
        while (isCharSkippable(c)) {
            if (c == '#') skipComment(); // skip to next line if there's a comment
            
            c = read();

            if (c == '\0') return;       // EOF, nothing to do
        }

        unread(c);                       // unread the last, non-skippable, char read
    }

    void unreadToken(const Token& token) {
        if (_savedToken.has_value()) {
            std::cout << "ERROR: there's already an unread token" << std::endl;
            exit(-1);
        }
        _savedToken = token;
    }


private:
    int _tabs;
    std::istream& _stream;
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

    Token readIdentifierOrKeyword(SourceLocation location) {
        std::string value = "";

        while (true) {
            if (!isalnum(peek()) && peek() != '_') break; // we know the first char is't a number
            value += read();
        }

        // if is not a keyword, return an identifier
        try { return Token(TokenTags::KEYWORD, KEYWORDS.at(value), location); } // at() throws exception if the key is not present
        catch (std::out_of_range& e) { return Token(TokenTags::IDENTIFIER, value, location); }
    }

    Token readStringToken(SourceLocation location) {
        std::string value = "";

        while (true) {
            char c = read();
            if (c == '"') break; // end quote, we read a string after reading the starting quote
            if (c == '\0') throw GrammarError(location, "unterminated string");
            value += c;
        }

        return Token(TokenTags::STRING_LITERAL, value, location);
    }

    Token readNumberToken(SourceLocation location) {
        std::string value = "";

        while (true) {
            char c = peek();
            if (!isdigit(c) && c != '.' && c != 'e' && c != 'E') break;

            value += read();
        }

        float number;
        try { number = stof(value); }
        catch (std::out_of_range& e) { throw GrammarError(location, value + " is out of float range"); }
        catch (std::invalid_argument& e) { throw GrammarError(location, value + " is not a valid number"); }
        // the following is needed because for example 1.2.3 is converted to 1.2 and we want to avoid this
        // we should also check if there are multiple 'e' or 'E' characters, and enable negative powers
        if (std::to_string(number) != value) throw GrammarError(location, value + " is not a valid number");

        return Token(TokenTags::NUMBER_LITERAL, number, location);
    }
};

std::string tokenToString(const Token& token) {
    switch (token.tag) {
        case TokenTags::KEYWORD:
            return "<keyword>";
        case TokenTags::IDENTIFIER:
            return token.value.string;
        case TokenTags::STRING_LITERAL:
            return "\"" + token.value.string + "\"";
        case TokenTags::NUMBER_LITERAL:
            return std::to_string(token.value.numberLiteral);
        case TokenTags::SYMBOL:
            return std::string(1, token.value.symbol);
        case TokenTags::STOP:
            return "<EOF>";
        default:
            return "<unknown>";
    }
}

std::string keywordToString(Keywords k) {
    switch (k) {
        case Keywords::CAMERA: return "camera";
        case Keywords::SPHERE: return "sphere";
        case Keywords::MATERIAL: return "material";
        case Keywords::DIFFUSE: return "diffuse";
        case Keywords::SPECULAR: return "specular";
        // ... aggiungi tutti gli altri enum
        default: return "<unknown>";
    }
}


class Scene {
public:
    World world;
    std::shared_ptr<Camera> camera;
    std::map<std::string, Material> materials;
    std::map<std::string, float> float_variables;
    std::set<std::string> overridden_variables;

    void expectSymbol(InputStream& input_file, const std::string& symbol);
    Keywords expectKeywords(InputStream& input_file, const std::vector<Keywords>& keywords);
    float expectNumber(InputStream& input_file, Scene& scene);
    std::string expectString(InputStream& input_file);
    std::string expectIdentifier(InputStream& input_file);

    static Vec3 parseVector(InputStream& input_file, Scene& scene);
    static Color parseColor(InputStream& input_file, Scene& scene);
    static std::shared_ptr<Texture> parseTexture(InputStream& input_file, Scene& scene);
    static std::shared_ptr<Material> parseMaterialDefinition(InputStream& input_file, Scene& scene); //solo il materiale (tipo, parametri)
    static std::pair<std::string, Material> parseNamedMaterial(InputStream& input_file, Scene& scene); // nome + materiale completo
    static Transformation parseTransformation(InputStream& input_file, Scene& scene);
    static Sphere parseSphere(InputStream& input_file, Scene& scene);
    static Plane parsePlane(InputStream& input_file, Scene& scene);
    static std::shared_ptr<Camera> parseCamera(InputStream& input_file, Scene& scene);
    static Scene parseScene(InputStream& input_file, const std::map<std::string, float>& variables = {});
};


// Implementazioni

void expectSymbol(InputStream& input_file, const std::string& symbol) {
    Token token = input_file.readToken();
    if (token.tag != TokenTags::SYMBOL || symbol.size() != 1 || token.value.symbol != symbol[0]) {
        throw GrammarError(token.location, "Expected a keyword, got '" + tokenToString(token) + "'");
    }
}

Keywords expectKeywords(InputStream& input_file, const std::vector<Keywords>& keywords) {
    Token token = input_file.readToken();
    if (token.tag != TokenTags::KEYWORD) {
        throw GrammarError(token.location, "Expected a keyword, got '" + tokenToString(token) + "'");
    }

    Keywords kw = token.value.keyword;
    if (std::find(keywords.begin(), keywords.end(), kw) == keywords.end()) {
        std::string expected_list;
        for (const auto& k : keywords)
            expected_list += keywordToString(k) + ", ";
        if (!expected_list.empty()) expected_list.pop_back(), expected_list.pop_back();
        throw GrammarError(token.location, "Expected one of {" + expected_list + "}, got '" + keywordToString(kw) + "'");
    }

    return kw;
}

float expectNumber(InputStream& input_file, Scene& scene) {
    Token token = input_file.readToken();

    if (token.tag == TokenTags::NUMBER_LITERAL) {
        return token.value.numberLiteral;
    } else if (token.tag == TokenTags::IDENTIFIER) {
        std::string name = token.value.string;
        if (!scene.float_variables.contains(name)) { //c++ 20 
            throw GrammarError(token.location, "Unknown variable '" + name + "'");
        }
        return scene.float_variables[name];
    }

    throw GrammarError(token.location, "Expected a number or variable, got '" + tokenToString(token) + "'");
}

std::string expectString(InputStream& input_file) {
    Token token = input_file.readToken();
    if (token.tag != TokenTags::STRING_LITERAL) {
        throw GrammarError(token.location, "Expected a string, got '" + tokenToString(token) + "'");
    }
    return token.value.string;
}

std::string expectIdentifier(InputStream& input_file) {
    Token token = input_file.readToken();
    if (token.tag != TokenTags::IDENTIFIER) {
        throw GrammarError(token.location, "Expected an identifier, got '" + tokenToString(token) + "'");
    }
    return token.value.string;
}


Vec3 Scene::parseVector(InputStream& input_file, Scene& scene) {
    scene.expectSymbol(input_file, "[");
    float x = scene.expectNumber(input_file, scene);
    scene.expectSymbol(input_file, ",");
    float y = scene.expectNumber(input_file, scene);
    scene.expectSymbol(input_file, ",");
    float z = scene.expectNumber(input_file, scene);
    scene.expectSymbol(input_file, "]");
    return Vec3(x, y, z);
}

Color Scene::parseColor(InputStream& input_file, Scene& scene) {
    scene.expectSymbol(input_file, "<");
    float r = scene.expectNumber(input_file, scene);
    scene.expectSymbol(input_file, ",");
    float g = scene.expectNumber(input_file, scene);
    scene.expectSymbol(input_file, ",");
    float b = scene.expectNumber(input_file, scene);
    scene.expectSymbol(input_file, ">");
    return Color(r, g, b);
}

Texture* parseTexture(InputStream& input_file, Scene& scene) {
    Keywords kw = expectKeywords(input_file, {
        Keywords::UNIFORM, Keywords::CHECKERED, Keywords::IMAGE
    });

    expectSymbol(input_file, std::string("("));

    Texture* result = nullptr;

    if (kw == Keywords::UNIFORM) {
        Color color = Scene::parseColor(input_file, scene);
        result = new UniformTexture(color);
    } else if (kw == Keywords::CHECKERED) {
        Color c1 = Scene::parseColor(input_file, scene);
        expectSymbol(input_file, ",");
        Color c2 = Scene::parseColor(input_file, scene);
        expectSymbol(input_file, ",");
        int steps = (int)expectNumber(input_file, scene);
        result = new CheckeredTexture(c1, c2, steps);
    } else if (kw == Keywords::IMAGE) {
        std::string filename = expectString(input_file);
        HDRImage image(filename);
        result = new ImageTexture(image);
    }

    expectSymbol(input_file, ")");
    return result;
}

std::shared_ptr<Material> parseMaterialDefinition(InputStream& input_file, Scene& scene) {
    Keywords kw = expectKeywords(input_file, {Keywords::DIFFUSE, Keywords::SPECULAR});
    expectSymbol(input_file, "(");

    std::shared_ptr<Texture> texture(parseTexture(input_file, scene));

    expectSymbol(input_file, ")");

    if (kw == Keywords::DIFFUSE)
        return std::make_shared<DiffuseMaterial>(texture);
    else
        return std::make_shared<SpecularMaterial>(texture);
}


std::pair<std::string, std::shared_ptr<Material>> parseNameMaterial(InputStream& input_file, Scene& scene) {
    std::string name = expectIdentifier(input_file);
    expectSymbol(input_file, "(");
    std::shared_ptr<Material> material = parseMaterialDefinition(input_file, scene);
    expectSymbol(input_file,")");
    return {name, material};
}


Transformation parseTransformation(InputStream& input_file, Scene& scene) {
    Transformation result;

    while (true) {
        Keywords kw = expectKeywords(input_file, {
            Keywords::IDENTITY,
            Keywords::TRANSLATION,
            Keywords::ROTATION_X,
            Keywords::ROTATION_Y,
            Keywords::ROTATION_Z,
            Keywords::SCALING
        });

        expectSymbol(input_file, "(");

        if (kw == Keywords::IDENTITY) {
        } else if (kw == Keywords::TRANSLATION) {
            result = result * translation(Scene::parseVector(input_file, scene));
        } else if (kw == Keywords::ROTATION_X) {
            result = result * rotation(expectNumber(input_file, scene), Axis::X);
        } else if (kw == Keywords::ROTATION_Y) {
            result = result * rotation(expectNumber(input_file, scene), Axis::Y);
        } else if (kw == Keywords::ROTATION_Z) {
            result = result * rotation(expectNumber(input_file, scene), Axis::Z);
        } else if (kw == Keywords::SCALING) {
            result = result * scaling(Scene::parseVector(input_file, scene));
        }

        expectSymbol(input_file, ")");

        Token t = input_file.readToken();
        if (t.tag != TokenTags::SYMBOL || t.value.symbol != '*') {
            input_file.unreadToken(t);
            break;
        }
    }

    return result;
}

Sphere parseSphere(InputStream& input_file, Scene& scene) {
    expectSymbol(input_file, "(");
    std::string mat = expectIdentifier(input_file);

    if (!scene.materials.contains(mat)) {  //c++20
        throw GrammarError(input_file._location, "Unknown material: " + mat);
    }

    expectSymbol(input_file, ",");
    Transformation transf = parseTransformation(input_file, scene);
    expectSymbol(input_file, ")");

    return Sphere(std::make_shared<Material>(scene.materials[mat]), transf);
}

Plane parsePlane(InputStream& input_file, Scene& scene) {
    expectSymbol(input_file, "(");
    std::string mat = expectIdentifier(input_file);

    if (!scene.materials.contains(mat)) { //c++20
        throw GrammarError(input_file._location, "Unknown material: " + mat);
    }

    expectSymbol(input_file, ",");
    Transformation transf = parseTransformation(input_file, scene);
    expectSymbol(input_file, ",");

    return Plane(std::make_shared<Material>(scene.materials[mat]), transf);
}

Camera* parseCamera(InputStream& input_file, Scene& scene) {
    expectSymbol(input_file, "(");
    Keywords kw = expectKeywords(input_file, {Keywords::PERSPECTIVE, Keywords::ORTHOGONAL});
    expectSymbol(input_file, ",");
    Transformation transf = parseTransformation(input_file, scene);
    expectSymbol(input_file, ",");
    float aspect = expectNumber(input_file, scene);
    expectSymbol(input_file, ",");
    float distance = expectNumber(input_file, scene);
    expectSymbol(input_file, ")");

    if (kw == Keywords::PERSPECTIVE)
        return new Camera("perspective", aspect, imageWidth, distance, transf);
    else
        return new OrthogonalCamera(aspect, transf);
}

Scene parseScene(InputStream& input_file, const std::unordered_map<std::string, float>& variables = {}) {
    Scene scene;
    scene.float_variables = variables;
    scene.overridden_variables.insert(variables.begin(), variables.end());

    while (true) {
        Token tok = input_file.readToken();
        if (tok.tag == TokenTags::STOP)
            break;

        if (tok.tag != TokenTags::KEYWORD)
            throw GrammarError(tok.location, "Expected a keyword");

        switch (tok.value.keyword) {
            case Keywords::FLOAT: {
                std::string name = expectIdentifier(input_file);
                auto loc = input_file._location();
                expectSymbol(input_file, "(");
                float val = expectNumber(input_file, scene);
                expectSymbol(input_file, ")");

                if (scene.float_variables.contains(name) && !scene.overridden_variables.contains(name)) { //c++20
                    throw GrammarError(loc, "Variable " + name + " cannot be redefined");
                }

                if (!scene.overridden_variables.contains(name)) //c++20
                    scene.float_variables[name] = val;

                break;
            }

            case Keywords::SPHERE:
                scene.world.addShape(parseSphere(input_file, scene));
                break;
            case Keywords::PLANE:
                scene.world.addShape(parsePlane(input_file, scene));
                break;
            case Keywords::CAMERA:
                if (scene.camera != nullptr)
                    throw GrammarError(tok.location, "Cannot define more than one camera");
                scene.camera = parseCamera(input_file, scene);
                break;
            case Keywords::MATERIAL: {
                auto [name, mat] = parseMaterial(input_file, scene);
                scene.materials[name] = mat;
                break;
            }

            default:
                throw GrammarError(tok.location, "Unexpected keyword");
        }
    }

    return scene;
}



#endif
