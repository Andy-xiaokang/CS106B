#ifndef MiniData_CSV_Included
#define MiniData_CSV_Included

#include <vector>
#include <unordered_map>
#include <string>
#include <istream>
#include <stdexcept>

/* Type representing data read from a CSV file containing a header row. Access is
 * provided as csv[row][column], where column can be specified either by an integer
 * or as one of the column headers.
 */
class CSV {
public:
    /* Parsing routines. */
    inline static CSV parse(std::istream& source);
    inline static CSV parseFile(const std::string& filename);

    /* Basic accessors. */
    inline std::size_t numRows() const;   // Doesn't include header
    inline std::size_t numCols() const;
    
    /* Header information. */
    inline std::vector<std::string> headers() const;

    /* Accessor proxy class. */
    class RowRef {
    public:
        inline std::string operator[] (std::size_t col) const;
        inline std::string operator[] (const std::string& colHeader) const;
    
    private:
        inline RowRef(const CSV* parent, std::size_t row);
        const CSV* mParent;
        std::size_t mRow;

        friend class CSV;
    };
    
    inline RowRef operator[] (std::size_t col) const;

private:
    /* The data. It's internally represented as a 2D grid of strings, along with
     * auxiliary column header data.
     *
     * The grid itself is represented in row-major order, and does not include the
     * column headers.
     */
    std::vector<std::string> mData;
    std::size_t              mRows;

    /* Column headers are encoded as a map from headers to indices, since the
     * primary operation we'll be supporting is mapping from a name to a column.
     */
    std::unordered_map<std::string, std::size_t> mColumnHeaders;
};

/* Type representing an error caused by a CSV issue. */
class CSVException: public std::logic_error {
public:
    inline CSVException(const std::string& message);
};







/* * * * * Implementation Below This Point * * * * */

#include <sstream>
#include <fstream>
#include <tuple>

namespace MiniData_CSVImpl {
    /* Reports an error. */
    [[ noreturn ]] inline void csvError(const std::string& message) {
        throw CSVException(message);
    }

    /* Reads a single CSV token from a source. Each token either
     *
     *  1. does not start with a quote, in which case we read up until the first comma, or
     *  2. starts with a quote, in which case we read to the upcoming close quote, watching for
     *     escaped quotes along the way.
     *
     * Empty entries are acceptable.
     */
    inline std::string readOneTokenFrom(std::istream& input) {
        /* Edge case: empty entries are fine. */
        if (input.peek() == ',') return "";
        
        /* If we don't start with a quote, read up until we do. */
        if (input.peek() != '"') {
            std::string result;           
            while (true) {
                int ch = input.peek();
                if (ch == EOF) return result;
                if (ch == ',') return result;
                result += char(input.get());
            }
        }
        
        /* We are looking a quoted string. Keep reading characters, keeping in mind that a close
         * quote might not actually be the end-of-string marker.
         */
        input.get(); // Skip quotation mark
        
        std::string result;
        while (true) {
            int ch = input.get();
            
            if (ch == EOF) csvError("Unterminated string literal.");
            else if (ch != '"') result += char(ch);
            else {
                int next = input.peek();
                if (next == EOF || next == ',') return result; // End of token
                else if (next == '"') {
                    /* Consume this character so we don't process it twice. */
                    input.get();
                    result += '"';
                } else csvError("Unexpected character found after quote.");
            }
        }
    }

    /* Tokenizes a line from a CSV file, returning a list of tokens within that line. */
    inline std::vector<std::string> tokenize(const std::string& line) {
        /* Edge case: we assume there are no empty lines even though in principle we could
         * envision a 0 x n data array. That likely just means something went wrong.
         */
        if (line.empty()) csvError("Empty line in CSV data.");
    
        /* Convert to a stream to make it easier to treat the characters as though they're a stream. */
        std::istringstream input(line);
        
        std::vector<std::string> result;
        while (true) {
            result.push_back(readOneTokenFrom(input));
            
            /* We should either see a comma or an EOF at this point. */
            if (input.peek() == EOF) return result;
            if (input.get()  != ',') csvError("Entries in CSV file aren't comma-separated?");
        }
    }

    /* Reads the first line of a CSV file, breaking it apart into headers. */
    inline std::unordered_map<std::string, std::size_t> readHeaders(std::istream& input) {
        std::string line;
        if (!std::getline(input, line)) csvError("Could not read header row from CSV source.");
        
        std::unordered_map<std::string, std::size_t> result;
        for (auto token: tokenize(line)) {
            if (result.count(token)) csvError("Duplicate column header: " + token);
            
            std::size_t index = result.size();
            result[token] = index;
        }
        
        return result;
    }
    
    /* Reads the body of a CSV file under the assumption that it has a certain number of
     * columns.
     *
     * The result is a pairing of the row-major-ordering of the data, along with the
     * number of rows in the data.
     */
    inline std::tuple<std::vector<std::string>, std::size_t> readBody(std::istream& input, std::size_t numCols) {
        /* We'll build the grid as a vector<vector<string>> and collapse it at the end. */
        std::vector<std::vector<std::string>> lines;
        for (std::string line; std::getline(input, line); ) {
            auto tokens = tokenize(line);
            if (tokens.size() != numCols) csvError("Lines have varying number of entries.");
            
            lines.push_back(tokens);
        }
        
        /* Flatten the list. */
        std::vector<std::string> result;
        for (std::size_t row = 0; row < lines.size(); row++) {
            for (std::size_t col = 0; col < numCols; col++) {
                result.push_back(lines[row][col]);
            }
        }
        return std::make_tuple(result, lines.size());
    }
}

inline CSV CSV::parse(std::istream& input) {
    CSV result;

    result.mColumnHeaders = MiniData_CSVImpl::readHeaders(input);
    std::tie(result.mData, result.mRows) = MiniData_CSVImpl::readBody(input, result.mColumnHeaders.size());

    return result;
}

inline CSV CSV::parseFile(const std::string& filename) {
    std::ifstream input(filename);
    if (!input) MiniData_CSVImpl::csvError("Cannot open file " + filename);

    return CSV::parse(input);
}

inline size_t CSV::numRows() const {
    return mRows;
}

inline size_t CSV::numCols() const {
    return mColumnHeaders.size();
}

inline std::vector<std::string> CSV::headers() const {
    std::vector<std::string> result(mColumnHeaders.size());
    for (const auto& entry: mColumnHeaders) {
        result[entry.second] = entry.first;
    }
    return result;
}

inline CSV::RowRef CSV::operator[] (std::size_t row) const {
    if (row >= numRows()) MiniData_CSVImpl::csvError("Row out of range.");
    
    return RowRef(this, row);
}

inline CSV::RowRef::RowRef(const CSV* parent, std::size_t row) : mParent(parent), mRow(row) {

}

inline std::string CSV::RowRef::operator[] (std::size_t col) const {
    if (col >= mParent->numCols()) MiniData_CSVImpl::csvError("Column out of range.");
    
    return mParent->mData[mParent->numCols() * mRow + col];
}
inline std::string CSV::RowRef::operator[] (const std::string& colHeader) const {
    auto itr = mParent->mColumnHeaders.find(colHeader);
    if (itr == mParent->mColumnHeaders.end()) MiniData_CSVImpl::csvError("Column not found: " + colHeader);

    return (*this)[itr->second];
}

inline CSVException::CSVException(const std::string& message) : std::logic_error(message) {
    // Handled in initialization list
}

#endif
