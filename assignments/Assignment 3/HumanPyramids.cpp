#include "HumanPyramids.h"
#include "error.h"
#include "grid.h"
#include "gridlocation.h"
#include "map.h"
using namespace std;

double weightOnBackOfHelper(int row, int col, int pyramidHeight, Grid<double>& weight) {
    if(row < 0 || col < 0 || col > row || row  >= pyramidHeight || col >= pyramidHeight) {
        error("the people are out of bounds");
    }
    // base case
    if (row == 0 && col == 0) {
        return 0.0;
    }
    if (weight[row][col] != 0.0) {
        return weight[row][col];
    }
    // recursive case
    double result;
    if (col == 0) {
        result = 80 + weightOnBackOfHelper(row-1, col, pyramidHeight, weight) / 2.0;
        weight[row][col] = result;
        return result;
    } else if (col == row) {
        result = 80 + weightOnBackOfHelper(row-1, col-1, pyramidHeight, weight) / 2.0;
        weight[row][col] = result;
        return result;
    } else {
        result = 160 + weightOnBackOfHelper(row-1, col-1, pyramidHeight, weight) / 2.0 + weightOnBackOfHelper(row-1, col, pyramidHeight, weight) / 2.0;
        weight[row][col] = result;
        return result;
    }
}

double weightOnBackOfHelper(int row, int col, int pyramidHeight, Map<GridLocation, double>& weight) {
    if(row < 0 || col < 0 || col > row || row  >= pyramidHeight || col >= pyramidHeight) {
        error("the people are out of bounds");
    }
    // base case
    if (row == 0 && col == 0) {
        return 0.0;
    }
    if (weight.containsKey({row, col})) {
        return weight.get({row, col});
    }
    // recursive case
    double result;
    if (col == 0) {
        result = 80 + weightOnBackOfHelper(row-1, col, pyramidHeight, weight) / 2.0;
        weight[{row, col}] = result;
        return result;
    } else if (col == row) {
        result = 80 + weightOnBackOfHelper(row-1, col-1, pyramidHeight, weight) / 2.0;
        weight[{row, col}] = result;
        return result;
    } else {
        result = 160 + weightOnBackOfHelper(row-1, col-1, pyramidHeight, weight) / 2.0 + weightOnBackOfHelper(row-1, col, pyramidHeight, weight) / 2.0;
        weight[{row, col}] = result;
        return result;
    }
}

double weightOnBackOf(int row, int col, int pyramidHeight) {
    // Map<GridLocation, double> weight;
    // return weightOnBackOfHelper(row, col, pyramidHeight, weight);
    Grid<double> weight(row+1, col+1, 0.0);
    return weightOnBackOfHelper(row, col, pyramidHeight, weight);
}


/* * * * * * Test Cases * * * * * */
#include "GUI/SimpleTest.h"

/* TODO: Add your own tests here. You know the drill - look for edge cases, think about
 * very small and very large cases, etc.
 */

STUDENT_TEST("if col > row, reports error") {
    EXPECT_ERROR(weightOnBackOf(3, 4, 5));
}

STUDENT_TEST("test people has only one parent") {
    EXPECT_EQUAL(weightOnBackOf(2, 0, 5), 120);
    EXPECT_EQUAL(weightOnBackOf(3, 0, 5), 140);
    EXPECT_EQUAL(weightOnBackOf(3, 3, 5), 140);
}

STUDENT_TEST(" test 5 rows") {
    EXPECT_EQUAL(weightOnBackOf(4, 2, 5), 500);
}

STUDENT_TEST("stress test") {
    EXPECT(weightOnBackOf(25, 10, 60) >= 1000);
}


/* * * * * * Test cases from the starter files below this point. * * * * * */

PROVIDED_TEST("Check Person E from the handout.") {
    /* Person E is located at row 2, column 1. */
    EXPECT_EQUAL(weightOnBackOf(2, 1, 5), 240);
}

PROVIDED_TEST("Function reports errors in invalid cases.") {
    EXPECT_ERROR(weightOnBackOf(-1, 0, 10));
    EXPECT_ERROR(weightOnBackOf(10, 10, 5));
    EXPECT_ERROR(weightOnBackOf(-1, 10, 20));
}

PROVIDED_TEST("Stress test: Memoization is implemented (should take under a second)") {
    /* TODO: Yes, we are asking you to make a change to this test case! Delete the
     * line immediately after this one - the one that starts with SHOW_ERROR - once
     * you have implemented memoization to test whether it works correctly.
     */
    //SHOW_ERROR("This test is configured to always fail until you delete this line from\n         HumanPyramids.cpp. Once you have implemented memoization and want\n         to check whether it works correctly, remove the indicated line.");

    /* Do not delete anything below this point. :-) */

    /* This will take a LONG time to complete if memoization isn't implemented.
     * We're talking "heat death of the universe" amounts of time. :-)
     *
     * If you did implement memoization but this test case is still hanging, make
     * sure that in your recursive function (not the wrapper) that your recursive
     * calls are to your new recursive function and not back to the wrapper. If you
     * call the wrapper again, you'll get a fresh new memoization table rather than
     * preserving the one you're building up in your recursive exploration, and the
     * effect will be as if you hadn't implemented memoization at all.
     */
    EXPECT(weightOnBackOf(100, 50, 200) >= 10000);
}

/* TODO: Add your own tests here. You know the drill - look for edge cases, think about
 * very small and very large cases, etc.
 */
