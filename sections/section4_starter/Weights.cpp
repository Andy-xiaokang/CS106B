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
#include "vector.h"
using namespace std;

/*
 * Function: isMeasurable
 * -----------------------
 * Write a recursive function that determines whether it is
 * possible to measure out the desired target amount with a
 * given set of weights, which is stored in the vector weights.
 */
bool isMeasurable(int target, const Vector<int>& weights) {
    // base case
    if (target == 0) {
        return true;
    }
    if (weights.isEmpty()) {
        return false;
    }
    // recursive case
    bool putRight, putLeft, drop;
    for (int i = 0; i < weights.size(); i++) {
        Vector<int> newWeights = weights;
        // choose and explore
        putRight = isMeasurable(target - weights[i], newWeights.subList(0,i) + newWeights.subList(i+1));
        putLeft = isMeasurable(target + weights[i], newWeights.subList(0,i) + newWeights.subList(i+1));
        drop = isMeasurable(target, newWeights.subList(0,i) + newWeights.subList(i+1));
    }
    return putRight || putLeft || drop;
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Provided Test: Run the provided examples.") {
    EXPECT_EQUAL(isMeasurable(2, {1, 3}), true);
    EXPECT_EQUAL(isMeasurable(5, {1, 3}), false);
    EXPECT_EQUAL(isMeasurable(6, {1, 3, 7}), true);
}
