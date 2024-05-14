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
using namespace std;

/*
 * Function: canVisitAllSites
 * ---------------------------
 * Write a function that takes as input a list of all the
 * sites you’d like to visit and an amount of free time
 * available to you and returns whether it’s possible to
 * visit all those sites in the allotted time (assume you’ve
 * already factored inthe cost of speaking at each site
 * and that you’re just concerned about the travel time.)
 */
double distance(const GPoint one, const GPoint two) {
    double deltaX = one.x - two.x;
    double deltaY = one.y - two.y;
    return sqrt(deltaX * deltaX + deltaY * deltaY);
}

Vector<GPoint> removeAt(Vector<GPoint> sites, int index) {
    GPoint p = sites.remove(index);
    return sites;
}

bool canVisitAllSitesHelper(const Vector<GPoint>& sites, double travelTimeAvailable, const GPoint& last, Vector<GPoint>& result) {
    if (sites.isEmpty() && travelTimeAvailable >=0) {
        cout << result << endl;
        return true;
    }
    for (int i = 0; i < sites.size(); i++) {
        double dist = distance(sites[i], last);
        if (dist < travelTimeAvailable && canVisitAllSitesHelper(removeAt(sites, i), travelTimeAvailable - dist, sites[i], result)) {
            result.insert(0, sites[i]);
            return true;
        }
    }
    return false;
}

bool canVisitAllSites(const Vector<GPoint>& sites, double travelTimeAvailable, Vector<GPoint>& result) {
    if (sites.isEmpty() && travelTimeAvailable >= 0) {
        cout << result << endl;
        return true;
    }
    for (int i = 0; i < sites.size(); i++) {
        if (canVisitAllSitesHelper(removeAt(sites, i), travelTimeAvailable, sites[i], result)) {
            result.insert(0, sites[i]);
            return true;
        }
    }
    return false;
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Provided Test: Simple examples!") {
    Vector<GPoint> sites1 = {{0, 0}, {5, 5}};
    Vector<GPoint> sites2 = {{0, 0}, {2, 2}, {5, 5}};
    Vector<GPoint> result1, result2;
    bool r1 = canVisitAllSites(sites1, 8, result1);
    bool r2 = canVisitAllSites(sites2, 8, result2);
    EXPECT_EQUAL(canVisitAllSites(sites1, 8, result1), true);
    EXPECT_EQUAL(canVisitAllSites(sites2, 8, result2), true);
}
