#ifndef __SceneFile__
#define __SceneFile__

#include <string>
#include <vector>
#include <exception>
#include <sstream>

class FileRegistry {
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

class SourceLocation {
public:
    SourceLocation(int file_index = -1, int line_num = 0, int col_num = 0)
        : fileIndex(file_index), lineNum(line_num), colNum(col_num) {}

    const std::string& fileName() const {
        return fileIndex >= 0 ? FileRegistry::getFile(fileIndex) : empty;
    }

    int getLine() const {return lineNum;}
    int getColumn() const {return colNum;}

private:
    int fileIndex;  // index into FileRegistry
    int lineNum;
    int colNum;

    inline static const std::string empty = "";
};

class GrammarError : public std::exception {
public:
    GrammarError(const SourceLocation& location, const std::string& message)
        : location(location), message(message) {
        std::ostringstream oss;
        oss << location.fileName() << ":" << location.getLine() << ":" << location.getColumn()
            << ": " << message;
        fullMessage = oss.str();
    }

    const SourceLocation& getLocation() const {
        return location;
    }

    const std::string& getMessage() const {
        return message;
    }

    const char* what() const noexcept override {
        return fullMessage.c_str();
    }

private:
    SourceLocation location;
    std::string message;
    std::string fullMessage;
};

#endif
