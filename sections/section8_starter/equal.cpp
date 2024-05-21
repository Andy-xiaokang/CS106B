/*
 * CS106B Section Handout Test Harness: Section 8
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified by Neel Kishnani for 
 * CS106B Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include <iostream>
#include "testing/SimpleTest.h"
#include "node.h"
#include "utility.h"
#include "error.h"
#include "set.h"
using namespace std;

/*
 * Function: areEqual
 * -------------------
 * Write a function that take as input pointers to the roots
 * of two binary trees (not necessarily binary search trees),
 * then returns whether the two trees have the exact same
 * shape and contents. Note that there is a solution for this
 * in this folder, but give this a try before looking!
 */

bool areEqual(Node* one, Node* two) {
    if (one == nullptr || two == nullptr) {
        return one == two;
    }
    return one->data == two->data && areEqual(one->left, two->left) && areEqual(one->right, two->right);
}

PROVIDED_TEST("Simple Tests for Tree Equality") {
    Node *a = nullptr;
    Node *b = nullptr;

    EXPECT(areEqual(a,b));

    a = createTreeFromVector({/* Level 1*/ 1});
    EXPECT(!areEqual(a,b));

    b = createTreeFromVector({/* Level 1*/ 1});
    EXPECT(areEqual(a,b));

    // same values but different structure
    freeTree(a);
    a = createTreeFromVector({/* Level 1*/ 1, /* Level 2*/ 3, EMPTY});

    freeTree(b);
    b = createTreeFromVector({/* Level 1*/ 1, /* Level 2*/ EMPTY, 3});

    EXPECT(!areEqual(a, b));

    freeTree(a);
    freeTree(b);

    // one tree is a subset of another but not equal
    a = createTreeFromVector({/* Level 1*/ 1, /* Level 2*/ 2, 3});
    b = createTreeFromVector({/* Level 1*/ 1, /* Level 2*/ 2, 3, /* Level 3*/ 4, EMPTY, EMPTY, 2});
    EXPECT(!areEqual(a, b));

    // complex trees that are equal
    freeTree(a);
    freeTree(b);

    a = createTreeFromVector({/* Level 1*/ 1, /* Level 2*/ 2, 3, /* Level 3*/ 4, 7, EMPTY, 2, /* Level 4 */ 5});
    b = createTreeFromVector({/* Level 1*/ 1, /* Level 2*/ 2, 3, /* Level 3*/ 4, 7, EMPTY, 2, /* Level 4 */ 5});
    EXPECT(areEqual(a, b));

    freeTree(a);
    freeTree(b);
}

