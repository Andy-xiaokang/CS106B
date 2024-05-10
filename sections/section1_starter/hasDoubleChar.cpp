#include "testing/SimpleTest.h"
using namespace std;


bool hasDoubledCharacter(string text) {
    for (int i = 1; i < text.size(); i++) {
        if (text[i] == text[i-1]) return true;
    }
    return false;
}

PROVIDED_TEST("Detects doubled characters") {
    EXPECT(hasDoubledCharacter("aa"));
    EXPECT(hasDoubledCharacter("bb"));
}

STUDENT_TEST("Strings without doubled characters") {
    EXPECT(!hasDoubledCharacter("abcd")); // Nothing doubled
    EXPECT(!hasDoubledCharacter("aba")); // a appears twice, but not consecutively
    EXPECT(!hasDoubledCharacter("Aa")); // Not technically the same character
}

STUDENT_TEST("Strings with doubled characters not at the front") {
    EXPECT(hasDoubledCharacter("abb")); // Back
    EXPECT(hasDoubledCharacter("abcddabc")); // Middle
}

STUDENT_TEST("Strings with doubled non-letter characters") {
    EXPECT(hasDoubledCharacter("**")); // Symbols
    EXPECT(hasDoubledCharacter("  ")); // Spaces
    EXPECT(hasDoubledCharacter("00")); // Numbers
    EXPECT(hasDoubledCharacter("!!")); // Punctuation
}

STUDENT_TEST("Short strings") {
    EXPECT(!hasDoubledCharacter("")); // Too short
    EXPECT(!hasDoubledCharacter("a")); // Too short
}
