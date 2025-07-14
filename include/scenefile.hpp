#ifndef __scenefile__
#define __scenefile__

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

/**
 * @brief Registry that manages unique file names by storing and indexing them.
 * 
 * Provides static methods to register a filename and retrieve it by its index.
 * Used to avoid duplicating file names in each token and to reference files by integer IDs.
 */
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



/**
 * @brief Represents a source code location with line and column numbers.
 * 
 * Optionally associates the location with a file index to retrieve the filename.
 */
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



// constants containing accepted symbols and keywords

const std::string SYMBOLS = ",()[]<>*";

/**
 * @brief Enumeration of all recognized keywords in the scene file format.
 * 
 * These keywords represent various constructs such as data types, transformations,
 * camera types, shapes, materials, and textures.
 */
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

// From https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key.
// Custom hash function to enable using user-defined types (enum class) as keys in a map
// Enum classes do not have a default hash function, so this struct provides a
// templated hash operator that casts the enum to its underlying integer type.
struct _EnumClassHash {
    template <typename T>
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



// data needed to construct the token type, since c++ doesn't have tagged unions
// (std::variant cannot differenciate between string literals and idetifiers
// since they're both strings)

enum class TokenTags {
    KEYWORD, IDENTIFIER, STRING_LITERAL, NUMBER_LITERAL, SYMBOL, STOP
};

/**
 * @brief Union to hold the actual value of a token.
 * 
 * Can store:
 * - Keywords enum value for keywords;
 * - std::string for identifiers and string literals;
 * - float for numeric literals;
 * - char for symbols.
 *
 */
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

/**
 * @brief Represents a lexical token with type, value, and source location.
 * 
 * Provides constructors for different token types and implements copy assignment.
 * The destructor handles proper cleanup of string members.
 * 
 * The toString() method returns a string representation of the token for debugging.
 */
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



/**
 * @brief Exception class for reporting grammar errors during parsing.
 * 
 * Stores the location (file, line, column) of the error along with a descriptive message.
 * 
 */
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



/**
 * @brief A helper class for reading characters and tokens from an input stream,
 *        tracking source location (file, line, column) for error reporting.
 * 
 * Supports reading, unreading and peeking characters,
 * reading and unreading tokens (keywords, identifiers, literals, symbols).
 * Also handles skipping whitespaces and comments.
 */
class InputStream {
public:
    InputStream(std::istream& stream, int fileIndex, int tabs = 4) // I'm writing in vscode with tab = 4 spaces
        : _location(fileIndex, 1, 1), _stream(stream), _tabs(tabs), _savedLocation() {}
    InputStream(std::istream& stream, const std::string& fileName, int tabs = 4)
        : _location(FileRegistry::registerFile(fileName), 1, 1), _stream(stream), _tabs(tabs), _savedLocation() {}

    /**
     * @brief Reads a character from the stream.
     * 
     * @return char 
     */
    char read();

    /**
     * @brief Puts the last character read back in the stream.
     * 
     * @param c The character to put back.
     */
    void unread(const char& c);

    /**
     * @brief Looks ahead one character without extracting it from the stream.
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
     * @brief Reads a token from the stream (keyword, variable identifier, string or number literal, symbol, or EOF).
     * 
     * @return Token 
    */
    Token readToken ();

    /**
     * @brief Puts the last token read back in the stream.
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

/**
 * @brief Represents a 3D scene including world geometry, lights, camera, materials, and variables.
 * 
 * This class is responsible for loading and parsing scene description files,
 * managing materials and variables, and storing the main elements used for rendering.
 */
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
        if (file.fail()) { // in the main we already check using CLI11, but you never know
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

#endif