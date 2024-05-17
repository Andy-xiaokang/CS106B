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
#include "utility.h"
#include "error.h"
using namespace std;

struct DoublyLinkedCell {
    string value; // Or whatever type of data goes here
    DoublyLinkedCell* next;
    DoublyLinkedCell* prev;
}; 

/*
 * Function: reverse
 * ------------------
 * Takes in a list and reverses it in place. Note that we're 
 * looking for both an iterative and a recursive implementation
 * here so feel free to test both out.
 */
void insertBefore(DoublyLinkedCell*& head, DoublyLinkedCell* beforeMe, DoublyLinkedCell* newCell) {
    if (beforeMe == head) {
        newCell->next = head;
        newCell->prev = nullptr;
        head->prev = newCell;
        head = newCell;
    }
    newCell->next = beforeMe;
    newCell->prev = beforeMe->prev;
    beforeMe->prev->next = newCell;
    beforeMe->prev = newCell;
}

/* * * * * Provided Tests Below This Point * * * * */
#include "vector.h"

PROVIDED_TEST("Small test for insertBefore") {
    DoublyLinkedCell* front = new DoublyLinkedCell;
    DoublyLinkedCell* middle = new DoublyLinkedCell;
    DoublyLinkedCell* back = new DoublyLinkedCell;

    front->next = middle;

    middle->prev = front;
    middle->next = back;

    back->prev = middle;

    DoublyLinkedCell* toAdd = new DoublyLinkedCell;

    insertBefore(front, middle, toAdd);
    EXPECT_EQUAL(middle->prev, toAdd);
    EXPECT_EQUAL(toAdd->next, middle);

    delete front;
    delete middle;
    delete back;
    delete toAdd;
}
