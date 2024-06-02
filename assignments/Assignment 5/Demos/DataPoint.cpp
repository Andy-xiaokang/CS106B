#include "DataPoint.h"
#include "strlib.h"
#include <sstream>
#include <iomanip>
using namespace std;

/* Utilities to read and write quoted strings.
 *
 * TODO: This should be replaced with the use of std::quoted as soon as
 * C++14 support is available on Windows.
 */
namespace {
    string quotedVersionOf(const string& source) {
        ostringstream result;
        result << '"';

        for (char ch: source) {
            /* Escape close quotes. */
            if (ch == '"') result << "\\\"";

            /* Escape slashes. */
            else if (ch == '\\') result << "\\\\";

            /* Print out any other printable characters. */
            else if (isgraph(ch) || ch == ' ') result << ch;

            /* Otherwise, escape it. */
            else {
                result << "\\x" << hex << setfill('0') << setw(2) << +static_cast<unsigned char>(ch);
            }
        }

        result << '"';
        return result.str();
    }

    /* Reads a quoted version of a string. */
    bool readQuoted(istream& in, string& out) {
        /* Read a character; it must be a quote. */
        char read;
        in >> read;

        if (!in || read != '"') return false;

        /* Keep reading until we get a close quote. */
        string result;
        while (true) {
            /* Can't read? That's a problem! */
            if (!in.get(read)) return false;

            /* If this is a quote, we're done. */
            else if (read == '"') break;

            /* Otherwise, if it's a slash, treat it as an escape. */
            else if (read == '\\') {
                /* Get the next character to see what we're supposed to do. */
                if (!in.get(read)) return false;

                /* Output slashes and quotes. */
                else if (read == '\\' || read == '"') result += read;

                /* Hex? Read two characters and decode them. */
                else if (read == 'x') {
                    string hexCode;
                    in >> setw(2) >> hexCode;
                    if (!in) return false;

                    /* Convert this to a number. */
                    try {
                        result += static_cast<char>(stringToInteger(hexCode, 16));
                    } catch (const ErrorException& e) {
                        return false;
                    }
                }
                /* Otherwise, we have no idea what this is. */
                else return false;
            }

            /* Otherwise, just append it. */
            else result += read;
        }

        out = result;
        return true;
    }
}

/* Equality comparison. */
bool operator== (const DataPoint& lhs, const DataPoint& rhs) {
    return lhs.name == rhs.name && lhs.weight == rhs.weight;
}

/* Inequality holds when the two points aren't equal. */
bool operator!= (const DataPoint& lhs, const DataPoint& rhs) {
    return !(lhs == rhs);
}

/* Prints a DataPoint to a stream. The output format is
 *
 *   { "name of the data point, \"properly escaped\"": pt.weight }
 */
ostream& operator<< (ostream& out, const DataPoint& pt) {
    ostringstream builder;
    builder << "{ " << quotedVersionOf(pt.name) << ", " << pt.weight << " }";
    return out << builder.str();
}

/* Reads a DataPoint from a stream. */
istream& operator>> (istream& in, DataPoint& result) {
    istream::sentry sentry(in);
    if (sentry) {
        /* Grab the next character; it should be an open brace. */
        char expected;
        in >> ws >> expected;
        if (!in || expected != '{') {
            in.setstate(ios::failbit);
            return in;
        }

        /* Extract the string. */
        DataPoint read;
        in >> ws;
        if (!readQuoted(in, read.name) || !in) {
            in.setstate(ios::failbit);
            return in;
        }

        /* Confirm there's a comma here. */
        in >> ws >> expected;
        if (!in || expected != ',') {
            in.setstate(ios::failbit);
            return in;
        }

        /* Read the weight. */
        in >> ws >> read.weight;
        if (!in) {
            in.setstate(ios::failbit);
            return in;
        }

        /* Read the close brace. */
        in >> ws >> expected;
        if (!in || expected != '}') {
            in.setstate(ios::failbit);
            return in;
        }

        result = read;
    }
    return in;
}
