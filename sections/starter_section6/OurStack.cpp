/*
 * CS106B Section Handout Test Harness: Section 6
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
#include "OurStack.h"
#include "error.h"
using namespace std;

const int kInitialSize = 4;

OurStack::OurStack() {
    elems = new int[kInitialSize];
    logicalSize = 0;
    allocatedSize = kInitialSize;
}

OurStack::~OurStack() {
    delete[] elems;
}

void OurStack::push(int value) {
    if (logicalSize == allocatedSize) {
        grow();
    }
    elems[logicalSize] = value;
    logicalSize++;
}

int OurStack::pop() {
    int value = peek();
    logicalSize--;
    return value;
}

int OurStack::peek() const {
    if (isEmpty()) {
        error("Empty stack!");
    }
    return elems[logicalSize - 1];
}

int OurStack::size() const {
    return logicalSize;
}

bool OurStack::isEmpty() const {
    return logicalSize == 0;
}

void OurStack::grow() {
    int* newArr = new int[allocatedSize * 2];
    for (int i = 0; i < logicalSize; i++) {
        newArr[i] = elems[i];
    }
    delete[] elems;
    elems = newArr;
    allocatedSize *= 2;
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Provided Test: Make sure the stack is functional.") {
    OurStack stack;
    EXPECT_EQUAL(stack.isEmpty(), true);
    for (int i = 0; i < 10000; i++) {
        stack.push(i);
    }
    EXPECT_EQUAL(stack.size(), 10000);
    for (int i = 0; i < 10000; i++) {
        int value = stack.pop();
        EXPECT_EQUAL(value, 9999-i);
    }
    EXPECT_EQUAL(stack.isEmpty(), true);
}
