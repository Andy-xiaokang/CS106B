/*
 * File: filelib.cpp
 * -----------------
 * This file implements the filelib.h interface.
 * Platform-dependent functions are handled through filelib_* functions
 * defined in filelibunix.cpp and filelibwindows.cpp.
 * 
 * @version 2016/11/20
 * - small bug fix in readEntireStream method (failed for non-text files)
 * @version 2016/11/12
 * - added fileSize, readEntireStream
 * @version 2016/08/12
 * - added second overload of openFileDialog that accepts path parameter
 * @version 2015/07/05
 * - removed static global Platform variable, replaced by getPlatform as needed
 * - moved appendSpace function to simpio
 * @version 2015/04/12
 * - added promptUserForFile overload without stream parameter
 * @version 2014/10/19
 * - alphabetized function declarations
 * - converted many funcs to take const string& rather than string for efficiency
 * - added listDirectory overload that returns a Vector
 * @version 2014/10/08
 * - removed 'using namespace' statement
 */

#include "filelib.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include "private/filelibunix.cpp"
#include "private/filelibwindows.cpp"
#include "gfilechooser.h"

/* Prototypes */

static void splitPath(const std::string& path, Vector<std::string> list);
static bool recursiveMatch(const std::string& str, int sx, const std::string& pattern, int px);

/* Implementations */

void createDirectory(const std::string& path) {
    return platform::filelib_createDirectory(expandPathname(path));
}

void createDirectoryPath(const std::string& path) {
    size_t cp = 0;
    if (path == "") return;
    std::string expandedPath = expandPathname(path);
    char sep = getDirectoryPathSeparator()[0];
    if (expandedPath.substr(1, 2) == ":\\") { // Windows drive letter followed by ':\'
        cp = 2;
    }
    while ((cp = expandedPath.find(sep, cp + 1)) != std::string::npos) {
       createDirectory(expandedPath.substr(0, cp));
    }
    createDirectory(expandedPath);
}

std::string defaultExtension(const std::string& filename, const std::string& ext) {
    std::string extCopy = ext;
    bool force = (extCopy[0] == '*');
    if (force) extCopy = extCopy.substr(1);
    int dot = -1;
    int len = filename.length();
    for (int i = 0; i < len; i++) {
        char ch = filename[i];
        if (ch == '.') dot = i;
        if (ch == '/' || ch == '\\') dot = -1;
    }
    if (dot == -1) {
        force = true;
        dot = len;
    }
    if (force) {
        return filename.substr(0, dot) + extCopy;
    } else {
        return filename;
    }
}

void deleteFile(const std::string& filename) {
    platform::filelib_deleteFile(expandPathname(filename));
}

std::string expandPathname(const std::string& filename) {
    return platform::filelib_expandPathname(filename);
}

bool fileExists(const std::string& filename) {
    return platform::filelib_fileExists(filename);
}

int fileSize(const std::string& filename) {
    std::ifstream input;
    input.open(filename.c_str(), std::ifstream::binary);
    if (input.fail()) {
        return -1;
    } else {
        input.seekg(0, std::ifstream::end);
        return (int) input.tellg();
    }
}

std::string getAbsolutePath(const std::string& path) {
    return platform::filelib_getAbsolutePath(path);
}

std::string getCurrentDirectory() {
    return platform::filelib_getCurrentDirectory();
}

std::string getDirectoryPathSeparator() {
    return platform::filelib_getDirectoryPathSeparator();
}

std::string getExtension(const std::string& filename) {
    int dot = -1;
    int len = filename.length();
    for (int i = 0; i < len; i++) {
        char ch = filename[i];
        if (ch == '.') dot = i;
        if (ch == '/' || ch == '\\') dot = -1;
    }
    if (dot == -1) {
        return "";
    } else {
        return filename.substr(dot);
    }
}

std::string getHead(const std::string& filename) {
    size_t slash = std::string::npos;
    size_t len = filename.length();
    for (size_t i = 0; i < len; i++) {
        char ch = filename[i];
        if (ch == '/' || ch == '\\') slash = i;
    }
    if (slash == std::string::npos) {
        return "";
    } else if (slash == 0) {
        return "/";
    } else {
        return filename.substr(0, slash);
    }
}

std::string getRoot(const std::string& filename) {
    int dot = -1;
    int len = filename.length();
    for (int i = 0; i < len; i++) {
        char ch = filename[i];
        if (ch == '.') dot = i;
        if (ch == '/' || ch == '\\') dot = -1;
    }
    if (dot == -1) {
        return filename;
    } else {
        return filename.substr(0, dot);
    }
}

std::string getSearchPathSeparator() {
    return platform::filelib_getSearchPathSeparator();
}

std::string getTail(const std::string& filename) {
    size_t slash = std::string::npos;
    size_t len = filename.length();
    for (size_t i = 0; i < len; i++) {
        char ch = filename[i];
        if (ch == '/' || ch == '\\') slash = i;
    }
    if (slash == std::string::npos) {
        return filename;
    } else {
        return filename.substr(slash + 1);
    }
}

std::string getTempDirectory() {
    return platform::filelib_getTempDirectory();
}

bool isDirectory(const std::string& filename) {
    return platform::filelib_isDirectory(expandPathname(filename));
}

bool isFile(const std::string& filename) {
    return platform::filelib_isFile(expandPathname(filename));
}

Vector<std::string> listDirectory(const std::string& path) {
    Vector<std::string> vec;
    platform::filelib_listDirectory(expandPathname(path), vec);
    return vec;
}

bool matchFilenamePattern(const std::string& filename, const std::string& pattern) {
    return recursiveMatch(filename, 0, pattern, 0);
}

bool openFile(std::ifstream& stream, const std::string& filename) {
    stream.clear();
    stream.open(expandPathname(filename));
    return !stream.fail();
}

bool openFile(std::ofstream& stream, const std::string& filename) {
    stream.clear();
    stream.open(expandPathname(filename));
    return !stream.fail();
}

std::string chooseFilenameDialog(const std::string& title,
                           const std::string& path,
                           const std::string& fileFilter) {
    return GFileChooser::showOpenDialog(title, path, fileFilter);
}

std::string findOnPath(const std::string& path,
                       const std::string& filename) {
    Vector<std::string> paths;
    splitPath(path, paths);
    for (std::string dir : paths) {
        std::string pathname = dir + "/" + filename;
        if (fileExists(pathname)) return pathname;
    }
    return "";
}

std::string readEntire(std::istream& input) {
    std::ostringstream output;
    while (true) {
        int ch = input.get();
        if (input.fail()) {
            break;
        }
        output.put(ch);
    }
    return output.str();
}

Vector<std::string> readLines(std::istream& input) {
    Vector<std::string> lines;
    while (true) {
        std::string line;
        getline(input, line);
        if (input.fail()) break;
        lines.add(line);
    }
    return lines;
}

void renameFile(const std::string& oldname, const std::string& newname) {
    std::string oldExpand = expandPathname(oldname);
    std::string newExpand = expandPathname(newname);
    rename(oldExpand.c_str(), newExpand.c_str());
}

void rewindStream(std::istream& input) {
    input.clear();                  // removes any current eof/failure flags
    input.seekg(0, std::ios::beg);  // tells the stream to seek back to the beginning
}

void setCurrentDirectory(const std::string& path) {
    return platform::filelib_setCurrentDirectory(path);
}

bool writeEntireFile(const std::string& filename,
                     const std::string& text,
                     bool append) {
    std::ofstream output;
    if (append) {
        output.open(filename.c_str(), std::ios_base::out | std::ios_base::app);
    } else {
        output.open(filename.c_str());
    }
    if (output.fail()) {
        return false;
    }
    output << text;
    output.close();
    return !output.fail();
}

/* Private functions */

static void splitPath(const std::string& path, Vector<std::string> list) {
    char sep = (path.find(';') == std::string::npos) ? ':' : ';';
    std::string pathCopy = path + sep;
    size_t start = 0;
    while (true) {
        size_t finish = pathCopy.find(sep, start);
        if (finish == std::string::npos) break;
        if (finish > start + 1) {
            list.add(pathCopy.substr(start, finish - start - 1));
        }
        start = finish + 1;
    }
}

static bool recursiveMatch(const std::string& str, int sx, const std::string& pattern, int px) {
    int slen = str.length();
    int plen = pattern.length();
    if (px == plen) return (sx == slen);
    char pch = pattern[px];
    if (pch == '*') {
        for (int i = sx; i <= slen; i++) {
            if (recursiveMatch(str, i, pattern, px + 1)) return true;
        }
        return false;
    }
    if (sx == slen) return false;
    char sch = str[sx];
    if (pch == '[') {
        bool match = false;
        bool invert = false;
        px++;
        if (px == plen) {
            error("matchFilenamePattern: missing ]");
        }
        if (pattern[px] == '^') {
            px++;
            invert = true;
        }
        while (px < plen && pattern[px] != ']') {
            if (px + 2 < plen && pattern[px + 1] == '-') {
                match |= (sch >= pattern[px] && sch <= pattern[px + 2]);
                px += 3;
            } else {
                match |= (sch == pattern[px]);
                px++;
            }
        }
        if (px == plen) {
            error("matchFilenamePattern: missing ]");
        }
        if (match == invert) return false;
    } else if (pch != '?') {
        if (pch != sch) return false;
    }
    return recursiveMatch(str, sx + 1, pattern, px + 1);
}
