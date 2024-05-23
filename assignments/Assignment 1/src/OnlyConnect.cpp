/* File: OnlyConnect.cpp
 *
 * TODO: Edit these comments to describe anything interesting or noteworthy in your implementation.
 *
 * TODO: Edit these comments to leave a puzzle for your section leader to solve!
 */
#include "OnlyConnect.h"
#include "GUI/SimpleTest.h"
#include "strlib.h"
using namespace std;

string onlyConnectizeHelper(string phrase, string & result) {
    // base case
    if (phrase.empty()) {
        return toUpperCase(result);
    }
    // recursive case
    else {
        if (!isalpha(phrase[0])) {
            return onlyConnectizeHelper(phrase.substr(1), result);
        } else {
            if (phrase[0] == 'a' || phrase[0] == 'e' || phrase[0] == 'i' || phrase[0] == 'o' || phrase[0] == 'u' ||
                phrase[0] == 'A' || phrase[0] == 'E' || phrase[0] == 'I' || phrase[0] == 'O' || phrase[0] == 'U') {
                return onlyConnectizeHelper(phrase.substr(1), result);
            } else {
                result += phrase[0];
                return onlyConnectizeHelper(phrase.substr(1), result);
            }
        }
    }
}
string onlyConnectize(string phrase) {
    string result = "";
    return onlyConnectizeHelper(phrase, result);
}






/* * * * * * Provided Test Cases * * * * * */

PROVIDED_TEST("Converts lower-case to upper-case.") {
    EXPECT_EQUAL(onlyConnectize("lowercase"), "LWRCS");
    EXPECT_EQUAL(onlyConnectize("uppercase"), "PPRCS");
}

PROVIDED_TEST("Handles non-letter characters properly.") {
    EXPECT_EQUAL(onlyConnectize("2.718281828459045"), "");
    EXPECT_EQUAL(onlyConnectize("'Hi, Mom!'"), "HMM");
}

PROVIDED_TEST("Handles single-character inputs.") {
    EXPECT_EQUAL(onlyConnectize("A"), "");
    EXPECT_EQUAL(onlyConnectize("+"), "");
    EXPECT_EQUAL(onlyConnectize("Q"), "Q");
}

STUDENT_TEST("test for empty string or large string") {
    EXPECT_EQUAL(onlyConnectize(""), "");
    EXPECT_EQUAL(onlyConnectize("to be or not to be, that is a question!"), toUpperCase("tbrnttbthtsqstn"));
}

STUDENT_TEST("test for quirk character") {
    EXPECT_EQUAL(onlyConnectize("!@#$%^&*()"), "");
}

/* TODO: You will need to add your own tests into this suite of test cases. Think about the sorts
 * of inputs we tested here, and, importantly, what sorts of inputs we *didn't* test here. Some
 * general rules of testing:
 *
 *    1. Try extreme cases. What are some very large cases to check? What are some very small cases?
 *
 *    2. Be diverse. There are a lot of possible inputs out there. Make sure you have tests that account
 *       for cases that aren't just variations of one another.
 *
 *    3. Be sneaky. Don't just try standard inputs. Try weird ones that you wouldn't expect anyone to
 *       actually enter, but which are still perfectly legal.
 *
 * Happy testing!
 */






