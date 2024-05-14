/*
 * CS106B Section Handout Test Harness: Section 4
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified for Qt and Ed by Neel
 * Kishnani for Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include <iostream>
#include "testing/SimpleTest.h"
#include "testing/TextUtils.h"
#include "set.h"
#include "map.h"
#include "error.h"
using namespace std;

/*
 * Function: matches
 * ------------------
 * Your task is to write a function that takes as input a
 * string and a pattern, then returns whether that string
 * matches the pattern.
 */
bool matchesHelper(const string& text, const string& pattern, int index1, int index2) {
    // base case
    if (index2 == pattern.size()) {  // consume up pattern
        return index1 == text.size();  // consume up text
    }
    // recursive case
    // case1: if pattern[index2] is alphabetic
    if (isalpha(pattern[index2])) {
        return index1 < text.size() &&
               text[index1] == pattern[index2] &&
               matchesHelper(text, pattern, index1 + 1, index2 + 1);
    } else if (pattern[index2] == '.') {
        return index1 < text.size() &&
               matchesHelper(text, pattern, index1 + 1, index2 + 1);
    } else if (pattern[index2] == '?') {
        return (index1 < text.size() && matchesHelper(text, pattern, index1 + 1, index2 + 1)) ||
               (matchesHelper(text, pattern, index1, index2 + 1));
    } else if (pattern[index2] == '*') {
        return (matchesHelper(text, pattern, index1, index2 + 1)) ||  // match none and skip '*'
               (index1 < text.size() && matchesHelper(text, pattern, index1 + 1, index2));   // match one and skip one character in text
    } else {
        error("no such pattern");
    }
}

bool matches(const string& text, const string& pattern) {
    return matchesHelper(text, pattern, 0, 0);
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Provided Test: Testing *") {
    EXPECT_EQUAL(matches("apple", "a*"), true);
    EXPECT_EQUAL(matches("apply", "a*"), true);
    EXPECT_EQUAL(matches("apoplexy", "a*"), true);
    EXPECT_EQUAL(matches("Amicus", "a*"), false);
    EXPECT_EQUAL(matches("banana", "a*"), false);
}

PROVIDED_TEST("Provided Test: Testing .") {
    EXPECT_EQUAL(matches("three", "th..."), true);
    EXPECT_EQUAL(matches("threshold", "th..."), false);
}

PROVIDED_TEST("Provided Test: Testing ?") {
    EXPECT_EQUAL(matches("color", "colo?r"), true);
    EXPECT_EQUAL(matches("colour", "colo?r"), true);
    EXPECT_EQUAL(matches("colouxr", "colo?r"), false);
}
