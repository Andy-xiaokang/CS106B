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

/**
 * Function: makeEmptyList
 * ------------------------
 * Creates a new doubly-linked list with a dummy head and tail, 
 * then returns a pointer to the head.
 */
DoublyLinkedCell* makeEmptyList() {
    DoublyLinkedCell* head = new DoublyLinkedCell;
    DoublyLinkedCell* tail = new DoublyLinkedCell;
    head->prev = nullptr;
    head->next = tail;
    tail->prev = head;
    tail->next = nullptr;
    return head;
}

/**
 * Function: printList
 * --------------------
 * Prints out the contents of a doubly-linked list whose dummy 
 * first and last elements are pointed at by the head and tail 
 * parameters.
 * 
 * Note: there is an implementation of this within this 
 * folder, but we recommend you code your own version 
 * for learning purposes!
 */
void printList(DoublyLinkedCell* head, DoublyLinkedCell* tail) {
    DoublyLinkedCell* curr = head->next;
    while (curr != tail) {
        cout << curr->value << endl;
        curr = curr->next;
    }
}

/**
 * Function: insertBefore
 * -----------------------
 * Takes in a newCell and a cell to insert the new cell before, 
 * and performs the pointer rewiring.
 */ 
void insertBefore(DoublyLinkedCell* newCell, DoublyLinkedCell* beforeMe) {
    newCell->next = beforeMe;
    newCell->prev = beforeMe->prev;
    beforeMe->prev->next = newCell;
    beforeMe->prev= newCell;
}

/**
 * Function: insertAfter
 * ----------------------
 * Takes in a newCell and a cell to insert the new cell after,
 * and perform the pointer rewiring.
 */
void insertAfter(DoublyLinkedCell* newCell, DoublyLinkedCell* afterMe) {
    newCell->next = afterMe->next;
    newCell->prev = afterMe;
    afterMe->next->prev = newCell;
    afterMe->next = newCell;
}

/**
 * Function: append
 * -----------------
 * Uses pointer rewiring to append newCell to the tail pointer.
 */ 
void append(DoublyLinkedCell* tail, DoublyLinkedCell* newCell) {
    insertBefore(tail, newCell);
}

/**
 * Function: prepend
 * ------------------
 * Uses pointer rewiring to prepend newCell to the tail head.
 */ 
void prepend(DoublyLinkedCell* head, DoublyLinkedCell* newCell) {
    insertAfter(head, newCell);
}

/**
 * Function: remove
 * -----------------
 * Splices out toRemove from the linked list.
 */ 
void remove(DoublyLinkedCell* toRemove) {
    toRemove->prev->next = toRemove->next;
    toRemove->next->prev = toRemove->prev;
    delete toRemove;
}

/**
 * Function: freeList
 * ------------------
 * Frees all memory associated with the linked list that 
 * head points to.
 * 
 * Note: there is an implementation of this within this 
 * folder, but we recommend you code your own version 
 * for learning purposes!
 */ 
void freeList(DoublyLinkedCell* head) {
    DoublyLinkedCell* curr = head->next;
    while (curr != nullptr) {
        DoublyLinkedCell* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
}

// Create more STUDENT_TESTs here!

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Small test for insertBefore") {
    DoublyLinkedCell* front = new DoublyLinkedCell;
    DoublyLinkedCell* back  = new DoublyLinkedCell;
    DoublyLinkedCell* toAdd = new DoublyLinkedCell;

    front->next = back;
    back->prev = front;

    insertBefore(toAdd, back);
    EXPECT_EQUAL(back->prev, toAdd);
    EXPECT_EQUAL(toAdd->next, back);
    EXPECT_EQUAL(toAdd->prev, front);
    EXPECT_EQUAL(front->next, toAdd);

    delete front;
    delete back;
    delete toAdd;
}

PROVIDED_TEST("Small test for insertAfter") {
    DoublyLinkedCell* front = new DoublyLinkedCell;
    DoublyLinkedCell* back  = new DoublyLinkedCell;
    DoublyLinkedCell* toAdd = new DoublyLinkedCell;

    front->next = back;
    back->prev = front;

    insertAfter(toAdd, front);
    EXPECT_EQUAL(back->prev, toAdd);
    EXPECT_EQUAL(toAdd->next, back);
    EXPECT_EQUAL(toAdd->prev, front);
    EXPECT_EQUAL(front->next, toAdd);

    delete front;
    delete back;
    delete toAdd;
}

PROVIDED_TEST("Small test for remove") {
    DoublyLinkedCell* front = new DoublyLinkedCell;
    DoublyLinkedCell* back  = new DoublyLinkedCell;
    DoublyLinkedCell* middle = new DoublyLinkedCell;

    front->next = middle;

    middle->prev = front;
    middle->next = back;

    back->prev = middle;

    remove(middle);
    EXPECT_EQUAL(back->prev, front);
    EXPECT_EQUAL(front->next, back);

    delete front;
    delete back;
    // middle should've been deleted in remove()
}
