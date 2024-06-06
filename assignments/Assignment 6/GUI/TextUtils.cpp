#include "TextUtils.h"
#include <sstream>
#include <iomanip>
using namespace std;

namespace {
    /* Quotes the given object with the specified escape characters. */
    string quotedVersionOf(const string& str, char quoteChar) {
        ostringstream result;
        result << quoteChar;

        for (char ch: str) {
            /* Escape close quotes. */
            if (ch == '\"') result << "\\\"";
            if (ch == '\'') result << "\\\'";

            /* Escape slashes. */
            else if (ch == '\\') result << "\\\\";

            /* Print out any other printable characters. */
            else if (isgraph(ch) || ch == ' ') result << ch;

            /* Otherwise, escape it. */
            else {
                result << "\\x" << hex << setfill('0') << setw(2) << +static_cast<unsigned char>(ch);
            }
        }

        result << quoteChar;
        return result.str();
    }
}


/* TODO: Once C++14 support is more widespread, replace with std::quoted. */
string quotedVersionOf(const string& source) {
    return quotedVersionOf(source, '"');
}
string quotedVersionOf(char source) {
    return quotedVersionOf(string(1, source), '\'');
}

/* Recursive base case. */
string format(const string& pattern) {
    /* If there's a replacement site, something is wrong. */
    if (pattern.find("%s") != string::npos) {
        error("Unmatched pattern string?");
    }
    return pattern;
}
