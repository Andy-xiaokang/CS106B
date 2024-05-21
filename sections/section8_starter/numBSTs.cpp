/*
 * CS106B Section Handout Test Harness: Section 8
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified by Neel Kishnani for 
 * CS106B Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include <iostream>
#include "testing/SimpleTest.h"
#include "node.h"
#include "utility.h"
#include "error.h"
#include "set.h"
using namespace std;

/*
 * Function: numBSTsOfSize
 * ------------------------
 * Takes as input a number n, then returns the
 * number of differently-shaped binary search
 * trees you can make out of n elements.
 */
int numBSTsOfSize(int n) {
    if (n == 0) {
        return 1;
    }
    // choose and explore
    int result = 0;
    for (int i = 0; i < n; i++) {  // the number of nodes in left subtree
        result += numBSTsOfSize(i) * numBSTsOfSize(n - i - 1);
    }
    return result;
}

PROVIDED_TEST("Simple tests for numBSTsOfSize function") {
    EXPECT_EQUAL(numBSTsOfSize(1), 1);
    EXPECT_EQUAL(numBSTsOfSize(2), 2);
    EXPECT_EQUAL(numBSTsOfSize(3), 5);
    EXPECT_EQUAL(numBSTsOfSize(4), 14);
    EXPECT_EQUAL(numBSTsOfSize(5), 42);
}
