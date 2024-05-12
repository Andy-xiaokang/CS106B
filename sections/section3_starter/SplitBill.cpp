/*
 * CS106B Section Handout Test Harness: Section 3
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
#include "error.h"
#include "map.h"
using namespace std;

/*
 * Function: listPossiblePayments
 * -------------------------------
 * Write a function that takes as input a total amount of money
 * to pay (in dollars) and a set of all the people who ordered
 * something, then lists off every possible way you could split
 * the bill, assuming everyone pays a whole number of dollars.
 */
// void listPossiblePaymentsHelper(int total, const Set<string>& peoples, const Map<string, int>& map) {
//     // base case
//     if (peoples.size() == 1) {
//         Map<string, int> final = map;
//         final[peoples.first()] = total;
//         cout << final << endl;
//         return;
//     }
//     // recursive case
//     for (int payment = 0; payment <= total; payment++) {
//         Map<string, int> updatedMap = map;
//         updatedMap[peoples.first()] = payment;
//         listPossiblePaymentsHelper(total - payment, peoples - peoples.first(), updatedMap);
//     }
// }

void listPossiblePaymentsHelper(int total, const Set<string>& peoples, Map<string, int> map) {
    // base case
    if (peoples.size() == 1) {
        map[peoples.first()] = total;
        cout << map << endl;
        return;
    }
    // recursive case
    for (int payment = 0; payment <= total; payment++) {
        map[peoples.first()] = payment;
        listPossiblePaymentsHelper(total - payment, peoples - peoples.first(), map);
    }
}

void listPossiblePayments(int total, const Set<string>& people) {
    if (total < 0) {
        error("total < 0");
    }
    if (people.size() < 1) {
        error("there must have one people");
    }
    Map<string, int> map;
    listPossiblePaymentsHelper(total, people, map);
    return;
}

/* * * * * Provided Tests Below This Point * * * * */

/*
 * Since this is a void function, we don't have a return value to verify with.
 * We will instead just run your code to make sure there is no obvious error.
 */
PROVIDED_TEST("Provided Test: Run the provided example. There should be 15 entries.") {
    Set<string> s = {"A", "B", "C"};
    listPossiblePayments(4, s);
}
