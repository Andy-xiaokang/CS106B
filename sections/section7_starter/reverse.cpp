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
 * Function: reverse
 * ------------------
 * Takes in a list and reverses it in place. Note that we're 
 * looking for both an iterative and a recursive implementation
 * here so feel free to test both out.
 */
void insert(Cell*& list, int value) {
    Cell* newNode = new Cell(value);
    if (list == nullptr) {
        list = newNode;
        return;
    }
    newNode->next = list;
    list = newNode;
}
void reverse(Cell*& list) {
    if (list == nullptr) {
        return;
    }
    Cell* head = list;
    list = nullptr;
    while (head != nullptr) {
        insert(list, head->value);
        Cell* tmp = head;
        head = head->next;
        delete tmp;
    }
}

/* * * * * Provided Tests Below This Point * * * * */
#include "vector.h"

PROVIDED_TEST("Small test for reverse") {
    Cell* list = createListFromVector({1, 2, 3, 4, 5});
    Cell* backwards = createListFromVector({5, 4, 3, 2, 1});
    reverse(list);
    printList(list);
    printList(backwards);
    EXPECT(listEqual(list, backwards));
    freeList(list);
    freeList(backwards);
}
