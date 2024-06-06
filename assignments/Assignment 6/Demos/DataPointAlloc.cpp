#include "Demos/DataPointAlloc.h"
#include "Demos/DataPoint.h"
#include "strlib.h"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

namespace {
    /* Hello! If the debugger took you here, it means
     * that there is some sort of memory error in your
     * program. Check the "application output" tab of
     * Qt Creator to see the error message that just
     * printed, then walk up in the call stack to see
     * where the specific error occurred.
     */
    [[ noreturn ]] void emergencyAbort(const char* message) {
        cerr << message << endl;
        abort();
    }
}

namespace DataPointUtils {
    /* When we do an allocation, we prepend space for one DataPoint before and one DataPoint
     * after the allocated region. Those regions are not actually going to hold DataPoints,
     * and instead will hold the following struct.
     */
    struct BlockBoundary {
        size_t blockSize;
        int type;
    };
    static_assert(sizeof(BlockBoundary) <= sizeof(DataPoint), "Internal error: contact course staff.");

    /* Constants used to track whether we did vector or scalar initialization. */
    const int kIsVector    = 0xA110C2;
    const int kIsScalar    = 0xA110C1;
    const int kDeallocated = 0xA110C0;

    /* Allocates a block of memory that contains one more DataPoint than requested.
     * That extra DataPoint is a sentinel that tells us whether we used the scalar
     * or vector alloc/dealloc function.
     */
    void* dataPointAlloc(size_t space, bool isVector) {
        /* Get more space than we need. Specifically, we want what we need, plus space
         * for the header and footer.
         */
        char* fullBlock  = static_cast<char *>(operator new(2 * sizeof(DataPoint) + space));
        char* headerAddr = fullBlock;
        char* footerAddr = fullBlock + sizeof(DataPoint) + space; // Skip header, skip payload
        void* result     = fullBlock + sizeof(DataPoint);

        /* Construct header and footer. */
        ::new (headerAddr) BlockBoundary{space, isVector? kIsVector : kIsScalar};
        ::new (footerAddr) BlockBoundary{space, isVector? kIsVector : kIsScalar};

        return result;
    }

    /* Deallocates a block of memory, checking to make sure the type of deallocation
     * performed was the right one.
     */
    void dataPointFree(void* memory, bool isVector) {\
        /* Get the header. It's DataPoint before the block of memory. */
        BlockBoundary* header = reinterpret_cast<BlockBoundary *>(((DataPoint*) memory) - 1);

        /* Confirm this one has the correct type. */
        if (header->type == kIsVector) {
            if (!isVector) {
                emergencyAbort("You are attempting to deallocate a block of memory that you allocated with "
                               "new[] using the delete operator. This will cause memory errors. Instead, "
                               "use the delete[] operator (with square brackets)."
                               "\n"
                               "Run your program with the debugger enabled and use the call stack to see "
                               "where this error occurred.");
            }
        } else if (header->type == kIsScalar) {
            if (isVector) {
                emergencyAbort("You are attempting to deallocate a block of memory that you allocated with "
                               "new using the delete[] operator. This will cause memory errors. Instead,"
                               "use the delete operator (without square brackets)."
                               "\n"
                               "Run your program with the debugger enabled and use the call stack to see "
                               "where this error occurred.");
            }
        } else if (header->type == kDeallocated) {
            emergencyAbort("You are attempting to delete memory that you have already deleted."
                           "\n"
                           "Run your program with the debugger enabled and use the call stack to see"
                           "where this error occurred.");
            abort();
        } else {
            emergencyAbort("We detected a memory error in your program when you tried deleting "
                           "memory. This error most likely occured somewhere earlier in the "
                           "program, but we're only just seeing that something bad happened "
                           "right now when we're doing the delete operation.\n\n"
                           "Make sure that all of your array accesses are in-bounds, that you "
                           "aren't deleting the same array twice, that you aren't mixing up "
                           "delete[] and delete, etc.\n\n"
                           "To see the specific spot in your code where this error occurred, "
                           "run the program again with the debugger and backtrace.");
        }

        /* Find the footer. */
        BlockBoundary* footer = reinterpret_cast<BlockBoundary*>(static_cast<char *>(memory) + header->blockSize);

        /* Confirm that the footer matches the header. */
        if (footer->blockSize != header->blockSize || footer->type != header->type) {
            cerr << "Something went wrong when you tried to deallocate memory. Specifically, the "
                    "memory right after the end of the allocated space has been modified since when "
                    "it was created. This might indicate writing off the end of an array, or could "
                    "be due to deallocating memory that wasn't allocated."
                    "\n"
                    "Run your program with the debugger enabled and use the call stack to see "
                    "where this error occurred." << endl;
            abort();
        }

        /* Clear the header and footer in case we use it again in the future. */
        header->type = footer->type = kDeallocated;
        header->blockSize = footer->blockSize = kDeallocated;

        /* Destroy the header and footer. */
        header->~BlockBoundary();
        footer->~BlockBoundary();

        /* Free the memory. */
        operator delete(header);
    }
}

/* Custom logic to make it easier to trap memory errors. */
namespace {
    const uint64_t kInitialized = 0xC5106BA6DA1ADA1A; // CS106B, A6, DATA DATA
    const uint64_t kDestroyed   = 0xC5106BA6DEADDA1A; // CS106B, A6, DEAD DATA

    void checkInitializationFlag(uint64_t value);
    void defaultConstruct(uint64_t& flag);
    void destruct(uint64_t& flag);
    void copyConstruct(DataPoint& lhs, const DataPoint& rhs);
    void moveConstruct(DataPoint& lhs, DataPoint&& rhs);
    void assign(DataPoint& lhs, DataPoint& rhs);
}

DataPoint::DataPoint() {
    /* If your code crashes on the line immediately below this one,
     * it likely indicates that you followed an invalid pointer - either
     * one that wasn't initialized, or a null pointer. Backtrace in the
     * debugger to see where the code this happened.
     */
    checkInitializationFlag(_initializationFlag);


    defaultConstruct(_initializationFlag);
}

DataPoint::~DataPoint() {
    /* If your code crashes on the line immediately below this one,
     * it likely indicates that you followed an invalid pointer - either
     * one that wasn't initialized, or a null pointer. Backtrace in the
     * debugger to see where the code this happened.
     */
    checkInitializationFlag(_initializationFlag);


    destruct(_initializationFlag);
}

DataPoint::DataPoint(const DataPoint& rhs) {
    /* If your code crashes on the lines immediately below this one,
     * it likely indicates that you followed an invalid pointer - either
     * one that wasn't initialized, or a null pointer. Backtrace in the
     * debugger to see where the code this happened.
     */
    checkInitializationFlag(_initializationFlag);
    checkInitializationFlag(rhs._initializationFlag);


    copyConstruct(*this, rhs);
}

DataPoint::DataPoint(DataPoint&& rhs) {
    /* If your code crashes on the lines immediately below this one,
     * it likely indicates that you followed an invalid pointer - either
     * one that wasn't initialized, or a null pointer. Backtrace in the
     * debugger to see where the code this happened.
     */
    checkInitializationFlag(_initializationFlag);
    checkInitializationFlag(rhs._initializationFlag);


    moveConstruct(*this, std::move(rhs));
}

DataPoint& DataPoint::operator=(DataPoint data) {
    /* If your code crashes on the line immediately below this one,
     * it likely indicates that you followed an invalid pointer - either
     * one that wasn't initialized, or a null pointer. Backtrace in the
     * debugger to see where the code this happened.
     */
    checkInitializationFlag(_initializationFlag);


    assign(*this, data);
    return *this;
}

DataPoint::DataPoint(const std::string& name, double weight) : name(name), weight(weight) {
    _initializationFlag = kInitialized;
}

namespace {
    /* No-op; just here to force the parameter to be copied. */
    void checkInitializationFlag(uint64_t) {

    }

    void defaultConstruct(uint64_t& initializationFlag) {
        initializationFlag = kInitialized;
    }

    void destruct(uint64_t& initializationFlag) {
        if (initializationFlag != kInitialized) {
            emergencyAbort("Your program has somehow tried to destroy a DataPoint object "
                           "that wasn't first initialized. This likely indicates some sort of "
                           "memory error in your code (accessing past the end of an array, mixing "
                           "up delete[] and delete, etc.)\n\n"
                           "Run this program in the debugger and backtrace to identify the spot "
                           "in your code where this was error was triggered.");
        }
        initializationFlag = kDestroyed;
    }

    void copyConstruct(DataPoint& lhs, const DataPoint& rhs) {
        if (rhs._initializationFlag != kInitialized) {
            emergencyAbort("You have tried to make a copy of a nonexistent DataPoint. "
                           "This sometimes happens if you try reading a DataPoint past "
                           "the end of an array, or before the start of the array.\n\n"
                           "Run your program with the debugger enabled and backtrace to "
                           "see the exact spot in the code where this error was triggered.");
        }
        lhs.weight = rhs.weight;
        lhs.name   = rhs.name;
        lhs._initializationFlag = kInitialized;
    }

    void moveConstruct(DataPoint& lhs, DataPoint&& rhs) {
        if (rhs._initializationFlag != kInitialized) {
            emergencyAbort("You have tried to make a copy of a nonexistent DataPoint. "
                           "This sometimes happens if you try reading a DataPoint past "
                           "the end of an array, or before the start of the array.\n\n"
                           "Run your program with the debugger enabled and backtrace to "
                           "see the exact spot in the code where this error was triggered.");
        }
        lhs.weight = std::move(rhs.weight);
        lhs.name   = std::move(rhs.name);
        lhs._initializationFlag = kInitialized;
    }

    void assign(DataPoint& lhs, DataPoint& rhs) {
        if (lhs._initializationFlag != kInitialized) {
            emergencyAbort("You have tried to assign to a nonexistent DataPoint. "
                           "This sometimes happens if you try writing a DataPoint past "
                           "the end of an array, or before the start of the array.\n\n"
                           "Run your program with the debugger enabled and backtrace to "
                           "see the exact spot in the code where this error was triggered.");
        }
        std::swap(lhs.weight, rhs.weight);
        std::swap(lhs.name,   rhs.name);
    }
}

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
    /* If your code crashes on the lines immediately below this one,
     * it likely indicates that you followed an invalid pointer - either
     * one that wasn't initialized, or a null pointer. Backtrace in the
     * debugger to see where the code this happened.
     */
    checkInitializationFlag(lhs._initializationFlag);
    checkInitializationFlag(rhs._initializationFlag);

    /* If your code crashes here, it may also indicate a memory error. */
    if (lhs._initializationFlag != kInitialized || rhs._initializationFlag != kInitialized) {
        emergencyAbort("You have tried comparing a nonexistent DataPoint "
                       "against another DataPoint. This likely indicates "
                       "a memory error. Backtrace in the debugger to see "
                       "where this error occurred.");
    }

    return lhs.name == rhs.name && lhs.weight == rhs.weight;
}

/* Inequality holds when the two points aren't equal. */
bool operator!= (const DataPoint& lhs, const DataPoint& rhs) {
    return !(lhs == rhs);
}

/* Prints a DataPoint to a stream. The output format is
 *
 *   { "name of the data point, \"properly escaped\"", pt.weight }
 */
ostream& operator<< (ostream& out, const DataPoint& pt) {
    /* If your code crashes on the line immediately below this one,
     * it likely indicates that you followed an invalid pointer - either
     * one that wasn't initialized, or a null pointer. Backtrace in the
     * debugger to see where the code this happened.
     */
    checkInitializationFlag(pt._initializationFlag);


    /* If your code crashes here, it means there's a memory error
     * somewhere in your code.
     */
    if (pt._initializationFlag != kInitialized) {
        emergencyAbort("You have attempted to print a nonexistent DataPoint "
                       "object. This is probably the result of a memory error "
                       "in your code. Run this program with the debugger engaged "
                       "and backtrace to locate where this occurred.");
    }


    ostringstream builder;
    builder << "{ " << quotedVersionOf(pt.name) << ", " << pt.weight << " }";
    return out << builder.str();
}

/* Reads a DataPoint from a stream. */
istream& operator>> (istream& in, DataPoint& result) {
    /* If your code crashes on the line immediately below this one,
     * it likely indicates that you followed an invalid pointer - either
     * one that wasn't initialized, or a null pointer. Backtrace in the
     * debugger to see where the code this happened.
     */
    checkInitializationFlag(result._initializationFlag);


    /* If your code crashes here, it means there's a memory error
     * somewhere in your code.
     */
    if (result._initializationFlag != kInitialized) {
        emergencyAbort("You have attempted to read into a nonexistent DataPoint "
                       "object. This is probably the result of a memory error "
                       "in your code. Run this program with the debugger engaged "
                       "and backtrace to locate where this occurred.");
    }


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
