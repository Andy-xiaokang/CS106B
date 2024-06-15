#pragma once

#include <string>
#include <sstream>
#include <utility>
#include <cstdlib>
#include "strlib.h"

/**
 * Given a number, returns a version of that number with commas separating the digits.
 *
 * @param val The number in question
 * @return The number with commas added.
 */
template <typename IntegerType> std::string addCommasTo(IntegerType val) {
    std::string asStr = std::to_string(val);
    const size_t length = asStr.length();

    std::string result;
    for (size_t i = 0; i < length; i++) {
        /* Run backwards through the source string so determining where commas go
         * becomes a lot easier.
         */
        result = asStr[length - 1 - i] + result;

        /* Put commas in provided we've already added three characters, but aren't
         * about to use all the digits up.
         */
        if (i % 3 == 2 && i < length - 1) {
            result = ',' + result;
        }
    }
    return result;
}

/* Given a quantity, returns a string of that quantity plus an appropriately-pluralized
 * version of what it is.
 */
template <typename ValueType>
std::string pluralize(const ValueType& value, const std::string& singular, const std::string& plural) {
    return addCommasTo(value) + " " + (value == 1? singular : plural);
}

/* Assume we suffix with s to pluralize unless specified otherwise. */
template <typename ValueType>
std::string pluralize(const ValueType& value, const std::string& singular) {
    return pluralize(value, singular, singular + "s");
}

/**
 * Given a string or character, quotes that string in a manner similar to C++14's std::quoted.
 *
 * @param input The text to escape.
 * @return A quoted version of that string.
 */
std::string quotedVersionOf(const std::string& input);
std::string quotedVersionOf(char input);

/**
 * Generic printf replacement. Given a string containing replacement sites of the form
 * %s and a number of arguments, replaces each of the %s sites with the appropriate
 * argument.
 *
 * If the number of arguments is wrong, this function will report an error.
 *
 * @param text The text, with placeholders.
 * @param args Replacements to use in those placeholders.
 */
std::string format(const std::string& pattern);

template <typename First, typename... Args>
std::string format(const std::string& pattern, First&& first, Args&&... args) {
    std::size_t toReplace = pattern.find("%s");
    if (toReplace == std::string::npos) {
        error("No pattern to replace?");
    }

    return    pattern.substr(0, toReplace)
            + std::to_string(std::forward<First>(first))
            + format(pattern.substr(toReplace + 2), args...);
}

template <typename... Args>
std::string format(const std::string& pattern, const std::string& first, Args&&... args) {
    std::size_t toReplace = pattern.find("%s");
    if (toReplace == std::string::npos) {
        error("No pattern to replace?");
    }

    return    pattern.substr(0, toReplace)
            + first
            + format(pattern.substr(toReplace + 2), args...);
}

template <typename... Args>
std::string format(const std::string& pattern, char first, Args&&... args) {
    std::size_t toReplace = pattern.find("%s");
    if (toReplace == std::string::npos) {
        error("No pattern to replace?");
    }

    return    pattern.substr(0, toReplace)
            + first
            + format(pattern.substr(toReplace + 2), args...);
}

/**
 * Joins a collection of strings together with the indicated conjunction. For example,
 * we might get back
 *
 *    A
 *    A and B
 *    A, B, and C
 *
 * @param strings The strings to join
 * @param conjunction Which conjunction to join them with
 * @return A string of the items joined via the conjunction
 */
template <typename Container>
std::string conjunctionJoin(const Container& container, const std::string& conjunction) {
    if (container.size() == 1) return *container.begin();
    if (container.size() == 2) return *container.begin() + " " + conjunction + " " + *next(container.begin());

    /* Form the string x1, x2, x3, ..., conjunction xn. */
    std::string result;
    for (auto itr = container.begin(); itr != container.end();  ++itr) {
        if (itr != container.begin()) result += ", ";
        if (next(itr) == container.end()) result += conjunction + " ";
        result += *itr;
    }

    return result;
}
