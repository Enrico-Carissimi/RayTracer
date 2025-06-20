#include <iostream>
#include "../utils.hpp"
#include "../Scenefile.hpp"

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

    std::istringstream ss2;
    ss2.str("1.2.3\n7e8888888");
    InputStream stream2(ss2, 0);

    testException(stream2, [](InputStream& s) {s.readToken();}); // two dots
    testException(stream2, [](InputStream& s) {s.readToken();}); // too big

    cout << "grammar errors are handled correctly" << endl;
}



int main() {
    testFileRegistry();
    testInputStream();
    testLexer();
    testThrowGrammarError();

    return 0;
}