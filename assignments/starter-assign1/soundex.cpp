/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include <cctype>
#include <fstream>
#include <string>
#include "console.h"
#include "strlib.h"
#include "filelib.h"
#include "simpio.h"
#include "vector.h"
#include "SimpleTest.h"
#include "soundex.h"
using namespace std;

/* This function is intended to return a string which
 * includes only the letter characters from the original
 * (all non-letter characters are excluded)
 *
 * check the first letter if it's not letter!
 */
string lettersOnly(string s) {
    string result = "";
    if (isalpha(s[0])) {
        result = charToString(s[0]);
    }
    for (int i = 1; i < s.length(); i++) {
        if (isalpha(s[i])) {
            result += s[i];
        }
    }
    return result;
}

string removeRedundant(string s) {
    string result = charToString(s[0]);
    for (int i = 0; i < s.length() -  1; i++) {
        if (s[i+1] == s[i]) {
            continue;
        } else {
            result += s[i+1];
        }
    }
    return result;
}

string getCode(string s) {
    string result = "";
    string upperS = toUpperCase(s);
    string start = charToString(upperS[0]);
    for (int i = 0; i < upperS.length(); i++) {
        if (stringContains("AEIOUHWY", upperS[i])) {
            result += "0";
        } else if (stringContains("BFPV", upperS[i])) {
            result += "1";
        } else if (stringContains("CGJKQSXZ", upperS[i])) {
            result += "2";
        } else if (stringContains("DT", upperS[i])) {
            result += "3";
        } else if (stringContains("L", upperS[i])) {
            result += "4";
        } else if (stringContains("MN", upperS[i])) {
            result += "5";
        } else if (stringContains("R", upperS[i])) {
            result += "6";
        }
    }
    result = removeRedundant(result);
    result = start + result.substr(1);
    return result;
}

void removeZeros(string& s) {
    string result = "";
    for (int i = 0; i < s.length(); i++) {
        if (s[i] != '0') {
            result += s[i];
        }
    }
    s = result;
}

string completeFinalCode(string s) {
    if (s.length() == 1) {
        return s + "000";
    } else if (s.length() == 2) {
        return s + "00";
    } else if (s.length() == 3) {
        return s + "0";
    } else if (s.length() == 4) {
        return s;
    } else {
        return s.substr(0, 4);
    }
}

STUDENT_TEST("") {
    string s = "C060";
    removeZeros(s);
    EXPECT_EQUAL(s, "C6");
    EXPECT_EQUAL(getCode("Curie"), "C060");
    EXPECT_EQUAL(removeRedundant("20600"), "2060");
    EXPECT_EQUAL(charToString('x').substr(1), "");
    EXPECT_EQUAL(completeFinalCode("C6"), "C600");
    EXPECT_EQUAL(completeFinalCode("O256"), "O256");
}

/* the soundex take a parameter string s and return its soundex code.
 */
string soundex(string s) {
    string letters = lettersOnly(s);
    string code = getCode(letters);
    removeZeros(code);
    code = completeFinalCode(code);
    return code;
}


/* the soundexSearch function find the same soundex name from the file
 * to your surname from the input
 */
void soundexSearch(string filepath) {
    // This provided code opens the specified file
    // and reads the lines into a vector of strings
    ifstream in;
    Vector<string> allNames;

    if (openFile(in, filepath)) {
        allNames = readLines(in);
    }
    cout << "Read file " << filepath << ", "
         << allNames.size() << " names found." << endl;

    // The names read from file are now stored in Vector allNames
    string surname;
    Vector<string> result;
    while (true) {
        surname = getLine("Enter a surname (RETURN to quit): ");
        if (surname == "") {
            cout << "All done!" << endl;
            return;
        }
        string soundexCode = soundex(surname);
        cout << "Soundex code is " << soundexCode << endl;
        for (string name : allNames) {
            if (soundexCode == soundex(name)) {
                result.add(name);
            }
        }
        result.sort();
        cout << "Matches from database: " << result << endl;
        result.clear();
    }
}


/* * * * * * Test Cases * * * * * */

STUDENT_TEST("test for lettersOnly() inspect the first letter") {
    string s = "*xiao kang";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "xiaokang");
    s = " xiao kang";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "xiaokang");
}


/* Please not add/modify/remove the PROVIDED_TEST entries below.
 * Place your student tests cases above the provided tests.
 */

PROVIDED_TEST("Test exclude of punctuation, digits, and spaces") {
    string s = "O'Hara";
    string result = lettersOnly(s);
    EXPECT_EQUAL(result, "OHara");
    s = "Planet9";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "Planet");
    s = "tl dr";
    result = lettersOnly(s);
    EXPECT_EQUAL(result, "tldr");
}

PROVIDED_TEST("Sample inputs from handout") {
    EXPECT_EQUAL(soundex("Curie"), "C600");
    EXPECT_EQUAL(soundex("O'Conner"), "O256");
}

PROVIDED_TEST("hanrahan is in lowercase") {
    EXPECT_EQUAL(soundex("hanrahan"), "H565");
}

PROVIDED_TEST("DRELL is in uppercase") {
    EXPECT_EQUAL(soundex("DRELL"), "D640");
}

PROVIDED_TEST("Liu has to be padded with zeros") {
    EXPECT_EQUAL(soundex("Liu"), "L000");
}

PROVIDED_TEST("Tessier-Lavigne has a hyphen") {
    EXPECT_EQUAL(soundex("Tessier-Lavigne"), "T264");
}

PROVIDED_TEST("Au consists of only vowels") {
    EXPECT_EQUAL(soundex("Au"), "A000");
}

PROVIDED_TEST("Egilsdottir is long and starts with a vowel") {
    EXPECT_EQUAL(soundex("Egilsdottir"), "E242");
}

PROVIDED_TEST("Jackson has three adjcaent duplicate codes") {
    EXPECT_EQUAL(soundex("Jackson"), "J250");
}

PROVIDED_TEST("Schwarz begins with a pair of duplicate codes") {
    EXPECT_EQUAL(soundex("Schwarz"), "S620");
}

PROVIDED_TEST("Van Niekerk has a space between repeated n's") {
    EXPECT_EQUAL(soundex("Van Niekerk"), "V526");
}

PROVIDED_TEST("Wharton begins with Wh") {
    EXPECT_EQUAL(soundex("Wharton"), "W635");
}

PROVIDED_TEST("Ashcraft is not a special case") {
    // Some versions of Soundex make special case for consecutive codes split by hw
    // We do not make this special case, just treat same as codes split by vowel
    EXPECT_EQUAL(soundex("Ashcraft"), "A226");
}
