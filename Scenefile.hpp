#ifndef __SceneFile__
#define __SceneFile__

#include <string>
#include <vector>
#include <exception>
#include <sstream>
#include <optional>
#include <unordered_map>

#include "utils.hpp"



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

private:
    int _tabs;
    std::istream& _stream;
    std::optional<char> _savedChar;
    SourceLocation _savedLocation;
    bool _peeking = false;

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

#endif
