#include "scenefile.hpp"

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
        camera = std::make_shared<Camera>("perspective", aspectRatio, (int)imageWidth, distance, transf);
    else
        camera = std::make_shared<Camera>("orthogonal", aspectRatio, (int)imageWidth, distance, transf);
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