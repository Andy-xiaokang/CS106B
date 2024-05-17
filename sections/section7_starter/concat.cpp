/*
 * CS106B Section Handout Test Harness: Section 7
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified by Neel Kishnani for 
 * Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */
#include <iostream>
#include <string>
#include "testing/SimpleTest.h"
#include "cell.h"
#include "utility.h"
#include "error.h"
using namespace std;

/*
 * Function: concat
 * -----------------
 * Takes in two lists and returns the concatenated version of them.
 * Note that this function header will be updated to be:
 *     void concat(Cell*& one, Cell* two);
 */
Cell* concat(Cell* one, Cell* two) {
    if (one == nullptr) {
        one = two;
    }
    Cell* curr = one;
    while (curr->next != nullptr) {
        curr = curr->next;
    }
    curr->next = two;
    return one;
}

void concat1(Cell* & one, Cell* two) {
    if (one == nullptr) {
        one = two;
        return;
    } else {
        Cell* curr = one;
        while (curr->next != nullptr) {
            curr = curr->next;
        }
        curr->next = two;
    }
}


/* * * * * Provided Tests Below This Point * * * * */
#include "vector.h"

PROVIDED_TEST("Small test for concat") {
    // Init Vectors
    Vector<int> one = {1, 2, 3, 4, 5};
    Vector<int> two = {6, 7, 8, 9, 10};
    Vector<int> combined = one + two;

    // Init Cell*
    Cell* oneList = createListFromVector(one);
    Cell* twoList = createListFromVector(two);
    Cell* combinedList = createListFromVector(combined);
    EXPECT(listEqual(concat(oneList, twoList), combinedList));
    printList(oneList);
    printList(twoList);
    Vector<int> three = {1, 2, 3, 4, 5};
    Vector<int> four = {6, 7, 8, 9, 10};
    Vector<int> combined1 = three + four;
    Cell* threeList = createListFromVector(three);
    Cell* fourList = createListFromVector(four);
    Cell* combinedList1 = createListFromVector(combined1);
    concat1(threeList, fourList);
    EXPECT(listEqual(threeList, combinedList1));
}
