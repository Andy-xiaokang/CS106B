#ifndef MiniData_Unicode_Included
#define MiniData_Unicode_Included

#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iterator>

/* Given a stream encoded in UTF-8, extracts one character from the stream. If the stream is
 * doesn't contain a proper encoding of a character - including if it's at EOF - this reports
 * an error by throwing a UTFException.
 */
inline char32_t readChar(std::istream& source);

/* Given a stream encoded in UTF-8, peeks at the next character from the stream. If the stream
 * doesn't contain a proper encoding of a character - including if it's at EOF - this reports
 * an error by throwing a UTFException.
 */
inline char32_t peekChar(std::istream& source);

/* Given a Unicode character in UTF-32, returns a UTF-8 representation of that character. */
inline std::string toUTF8(char32_t ch);

/* Given a string representing a single UTF-8 character, returns a char32_t representation
 * of that character. If the string does not represent a single UTF-8 character, throws
 * a UTFException.
 */
inline char32_t fromUTF8(const std::string& ch);

/* Given a Unicode character in UTF-32, returns a string representing a series of UTF-16 
 * escape sequences for that character. This string will either have the format
 * \uXXXX (for characters that don't need surrogates) or \uXXXX\uXXXX (for characters that
 * do.)
 */
inline std::string utf16EscapeFor(char32_t ch);

/* Given a string pointing at an escape sequence of the form \uHHHH (or \uHHHH\uHHHH for a
 * surrogate pair), reads the escape sequence(s) and returns the resulting character. If
 * the stream doesn't contain a sequence formatted this way - including if it's at EOF -
 * this reports an error by throwing a UTFException.
 */
inline char32_t readUTF16EscapedChar(std::istream& source);

/* Type representing an exception generated during UTF coding. */
class UTFException: public std::logic_error {
public:
    inline UTFException(const std::string& message);
};

/* Wrapper type supporting iteration over the characters in a string. This
 * allows you to say something like
 *
 *     for (char32_t ch: utf8Reader(str)) {
 *          ...
 *     }
 */
class utf8Reader {
public:
    explicit utf8Reader(const std::string& str) : in(str) {}

    class const_iterator;
    const_iterator begin();
    const_iterator end();

private:
    std::istringstream in;
    friend class const_iterator;
};










/* * * * * Implementation Below This Point * * * * */

#include <sstream>
#include <iomanip>
#include <cctype>

namespace MiniData_UnicodeImpl {
    /* Reports a UTF error. */
    [[ noreturn ]] inline void utfError(const std::string& message) {
        throw UTFException(message);
    }

    /* Gets the next raw character from a stream, reporting an error() if unable to do so. */
    inline char get(std::istream& input) {
        char result;
        if (!input.get(result)) utfError("Unexpected end of stream.");

        return result;
    }
    
    /* Returns whether a given byte is a follow byte (that is, a byte starting with 10). */
    inline bool isFollowByte(char byte) {
        return (byte & 0b11000000) == 0b10000000;
    }
    
    /* Converts a given character to a hex value. */
    inline std::string toHex(char ch) {
        std::ostringstream builder;
        builder << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(ch));
        return builder.str();
    }
    
    /* Given a stream encoded in UTF-8, reads the bytes that compose a single Unicode
     * character.
     */
    inline std::string nextBytesFrom(std::istream& source) {
        char header = get(source);
        std::string result(1, header);
        
        /* If this character doesn't have a high bit set, that's all there is to read. */
        if ((header & 0b10000000) == 0) return result;
        
        /* Otherwise, see how many characters there are to read. */
        std::size_t followBytes = 0;
        if      ((header & 0b11100000) == 0b11000000) followBytes = 1;
        else if ((header & 0b11110000) == 0b11100000) followBytes = 2;
        else if ((header & 0b11111000) == 0b11110000) followBytes = 3;
        else utfError("Byte header doesn't match UTF-8 patterns.");
        
        for (std::size_t i = 0; i < followBytes; i++) {
            char next = get(source);
            if (!isFollowByte(next)) utfError("Expected follow byte, got " + toHex(next));
            
            result += next;
        }
        
        return result;        
    }
    
    /* Given a string representing the bytes of a UTF-8 encoded character, decodes those
     * bytes into a single character.
     */
    inline char32_t decode(const std::string& bytes) {
        if (bytes.empty()) utfError("Empty byte string?");
        
        /* If the first byte starts with a zero bit, we just return it as-is. */
        if ((bytes[0] & 0b10000000) == 0) {
            if (bytes.size() != 1) utfError("Wrong number of bytes for 7-bit code point.");
            
            return bytes[0];
        }
        
        /* If the first byte begins with 110xxxxx, then we need to decode an 11-bit number
         * of the form 110bbbbb 10bbbbbb
         */
        if ((bytes[0] & 0b11100000) == 0b11000000) {
            if (bytes.size() != 2) utfError("Wrong number of bytes for 11-bit code point.");
            if (!isFollowByte(bytes[1])) utfError("Paired byte has wrong header.");
            
            return ((bytes[0] & 0b00011111) << 6) +
                   ((bytes[1] & 0b00111111) << 0);
        }
        
        /* If the first byte begins with 1110xxxx, then we need to decode a 16-bit number
         * of the form 1110bbbb 10bbbbbb 10bbbbbb.
         */
        if ((bytes[0] & 0b11110000) == 0b11100000) {
            if (bytes.size() != 3) utfError("Wrong number of bytes for 16-bit code point.");
            if (!isFollowByte(bytes[1])) utfError("First paired byte has wrong header.");
            if (!isFollowByte(bytes[2])) utfError("Second paired byte has wrong header.");
            
            return ((bytes[0] & 0b00001111) << 12) + 
                   ((bytes[1] & 0b00111111) <<  6) +
                   ((bytes[2] & 0b00111111) <<  0);
        }
        
        /* If the first byte begins with 11110xxx, then we need to decode a 21-bit number
         * of the form 11110bbb 10bbbbbb 10bbbbbb 10bbbbbb.
         */
        if ((bytes[0] & 0b11111000) == 0b11110000) {
            if (bytes.size() != 4) utfError("Wrong number of bytes for 21-bit code point.");
            if (!isFollowByte(bytes[1])) utfError("First paired byte has wrong header.");
            if (!isFollowByte(bytes[2])) utfError("Second paired byte has wrong header.");
            if (!isFollowByte(bytes[3])) utfError("Third paired byte has wrong header.");
            
            return ((bytes[0] & 0b00000111) << 18) + 
                   ((bytes[1] & 0b00111111) << 12) +
                   ((bytes[2] & 0b00111111) <<  6) +
                   ((bytes[3] & 0b00111111) <<  0);
        }
        
        utfError("Not sure how to handle byte " + toHex(bytes[0]));
    }
    
    /* Given a 16-bit value, writes out an escape sequence for it. */
    inline std::string oneEscapeUTF16For(uint16_t value) {
        std::ostringstream builder;
        builder << "\\u" << std::hex << std::setfill('0') << std::setw(4) << value;
        return builder.str();
    }
    
    /* Given a character that requires a surrogate pair, returns the surrogate components. */
    inline uint16_t highSurrogateFor(char32_t ch) {
        return ((ch - 0x10000) >> 10) + 0xD800;            // Top 10 bits
    }
    inline uint16_t lowSurrogateFor(char32_t ch) {
        return ((ch - 0x10000) & 0b1111111111) + 0xDC00;   // Lower 10 bits
    }
    
    /* Given a surrogate pair, assembles the pair back into a single Unicode character. */
    inline char32_t assembleSurrogates(uint16_t high, uint16_t low) {
        return ((high - 0xD800) << 10) + (low - 0xDC00) + 0x10000;
    }
    
    /* Given a stream, reads an escape sequence of the form \uXXXX and returns the value
     * encoded this way. It may be part of a surrogate pair, so what's returned may not
     * actually be a valid character.
     */
    inline char32_t readOneUTF16Escape(std::istream& input) {
        /* Confirm we start with \u. */
        if (get(input) != '\\') utfError("Expected \\u.");
        if (get(input) != 'u')  utfError("Expected \\u.");
        
        /* Read four bytes of hex. */
        std::string builder;
        for (int i = 0; i < 4; i++) {
            char next = get(input);
            if (!isxdigit(next)) utfError("Expected hexadecimal digit, got " + std::string(1, next));
            
            builder += next;
        }
        
        /* Convert those bytes to an integer. */
        return char32_t(stoi(builder, nullptr, 16));
    }
}

inline char32_t readChar(std::istream& source) {
    return MiniData_UnicodeImpl::decode(MiniData_UnicodeImpl::nextBytesFrom(source));
}

inline char32_t peekChar(std::istream& source) {
    /* Read the bytes and decode them. */
    auto bytes = MiniData_UnicodeImpl::nextBytesFrom(source);
    auto result = MiniData_UnicodeImpl::decode(bytes);
    
    /* Put the bytes back. */
    for (std::size_t i = 0; i < bytes.size(); i++) {
        source.unget();
        if (!source) MiniData_UnicodeImpl::utfError("Couldn't unget enough characters.");
    }
    
    return result;
}

inline std::string utf16EscapeFor(char32_t ch) {
    /* If this character is in the range where we can just directly convert it, go do so. */
    if (ch <= 0xD7FF || (ch >= 0xE000 && ch <= 0xFFFF)) {
        return MiniData_UnicodeImpl::oneEscapeUTF16For(ch);
    } else {
        return MiniData_UnicodeImpl::oneEscapeUTF16For(MiniData_UnicodeImpl::highSurrogateFor(ch)) +
               MiniData_UnicodeImpl::oneEscapeUTF16For(MiniData_UnicodeImpl::lowSurrogateFor(ch));
    }
}

inline char32_t readUTF16EscapedChar(std::istream& source) {
    char32_t result = MiniData_UnicodeImpl::readOneUTF16Escape(source);
    
    /* If what we read is pair of a high surrogate, read the next half and reassemble it. */
    if (result >= 0xD800 && result <= 0xDFFF) {
        if (result >= 0xDC00) MiniData_UnicodeImpl::utfError("Read second half of surrogate pair with no matching first half?");
        
        return MiniData_UnicodeImpl::assembleSurrogates(result, MiniData_UnicodeImpl::readOneUTF16Escape(source));
    }
    
    return result;
}


inline std::string toUTF8(char32_t charCode) {
    std::ostringstream result;

    /* Anything 7 bits or less just gets directly mapped to itself. */
    if (charCode < (1u << 7)) {
        result << char(charCode);
    }
    /* Anything using 11 bits or less gets broken into
     * 110xxxxx 10xxxxxx
     */
    else if (charCode < (1u << 11)) {
        uint32_t highFive = charCode >> 6;
        uint32_t lowSix   = charCode & 0b111111;

        result << char(highFive | 0b11000000) << char(lowSix | 0b10000000);
    }
    /* Anything using 16 bits or less gets broken into
     * 1110xxxx 10xxxxxx 10xxxxxx
     */
    else if (charCode < (1u << 16)) {
        uint32_t highFour = charCode >> 12;
        uint32_t midSix   = (charCode >> 6) & 0b111111;
        uint32_t lowSix   = charCode & 0b111111;

        result << char(highFour | 0b11100000)
               << char(midSix | 0b10000000)
               << char(lowSix | 0b10000000);
    }
    /* Anything using 21 bits or less gets broken into
     * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
     */
    else if (charCode < (1u << 22)) {
        uint32_t highThree = charCode >> 18;
        uint32_t topSix    = (charCode >> 12) & 0b111111;
        uint32_t midSix    = (charCode >>  6) & 0b111111;
        uint32_t lowSix    = charCode & 0b111111;

        result << char(highThree | 0b11110000)
               << char(topSix | 0b10000000)
               << char(midSix | 0b10000000)
               << char(lowSix | 0b10000000);
    }
    else {
        MiniData_UnicodeImpl::utfError("Unicode value out of range: " + std::to_string(charCode));
    }

    return result.str();
}

inline char32_t fromUTF8(const std::string& str) {
    std::istringstream input(str);
    char32_t result = readChar(input);
  
    if (input.get() != EOF) MiniData_UnicodeImpl::utfError("Unexpected bytes found in fromUTF8.");
  
    return result;
}

inline UTFException::UTFException(const std::string& message) : std::logic_error(message) {

}

class utf8Reader::const_iterator: public std::iterator<std::input_iterator_tag, const char32_t> {
public:
    const_iterator() = default;

    /* We're only equal if we're end-of-range iterators or if both of us are at the end. */
    bool operator== (const_iterator rhs) const {
        bool us   =     !owner ||     done;
        bool them = !rhs.owner || rhs.done;

        return us == them;
    }
    bool operator!= (const_iterator rhs) const {
        return !(*this == rhs);
    }

    reference operator* () const {
        return staged;
    }

    const_iterator& operator++() {
        read();
        return *this;
    }

    const_iterator operator++(int) {
        auto result = *this;
        ++*this;
        return result;
    }

private:
    const_iterator(utf8Reader* source) : owner(source) {
        if (owner) {
            read();
        } else {
            done = true;
        }
    }

    void read() {
        if (owner->in.peek() != EOF) {
            staged = readChar(owner->in);
        } else {
            done = true;
        }
    }

    friend class utf8Reader;
    utf8Reader* owner = nullptr;
    char32_t staged;
    bool done = false;
};

inline utf8Reader::const_iterator utf8Reader::begin() {
    return const_iterator(this);
}

inline utf8Reader::const_iterator utf8Reader::end() {
    return const_iterator();
}

#endif
