/*
 * CS106B Section Handout Test Harness: Section 1
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified for Qt and Ed by Neel
 * Kishnani for Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include "testing/SimpleTest.h"
using namespace std;

/*
 * Function: peopleInPyramidOfHeight
 * ---------------------------------
 * Takes as input the height of the human pyramid and 
 * returns the number of people in that pyramid. A 
 * human pyramid is a triangular stack of a bunch of
 * people where each person (except the one at the top)
 * supports their weight on the two people below them.
 */
int peopleInPyramidOfHeight(int n) {
    if (n == 0) {
        return 0;
    }
    return n + peopleInPyramidOfHeight(n-1);
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("peopleInPyamidOfHeight tests") {
    EXPECT_EQUAL(peopleInPyramidOfHeight(0), 0);
    EXPECT_EQUAL(peopleInPyramidOfHeight(1), 1);
    EXPECT_EQUAL(peopleInPyramidOfHeight(2), 3);
    EXPECT_EQUAL(peopleInPyramidOfHeight(3), 6);
}
