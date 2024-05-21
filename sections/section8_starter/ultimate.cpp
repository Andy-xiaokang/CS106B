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
 * Ultimate and Penultimate Values 
 * --------------------------------------------------------
 * Write a function that takes as input a pointer to the root
 * of a (nonempty) binary search tree, then returns a pointer
 * to the node containing the largest value in the BST. Then,
 * write a function that takes as input a pointer to the root
 * of a BST containing at least two nodes, then returns a
 * pointer to the node containing the second-largest value
 * in the BST.
 */
Node* biggestNodeIn(Node* root) {
    // base case
    if (root->right == nullptr) {
        return root;
    }
    return biggestNodeIn(root->right);
}

Node* secondBiggestNodeIn(Node* root) {
    // base case
    if (root->left != nullptr && root->right == nullptr) {
        return biggestNodeIn(root->left);
    }
    if (root->right->right == nullptr) {
        return root;
    }
    return secondBiggestNodeIn(root->right);
}

PROVIDED_TEST("Simple tests for biggestNodeIn function") {
    Node* tree = createTreeFromVector({/* Level 1*/ 5, /* Level 2*/ 3, 6, /* Level 3*/ 1, 4, EMPTY, 9, /* Level 4*/ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 8, 10});
    Node* answer = biggestNodeIn(tree);
    EXPECT_EQUAL(answer->data, 10);
    freeTree(tree);
}

PROVIDED_TEST("Simple tests for secondBiggestNodeIn function") {
    Node* tree = createTreeFromVector({/* Level 1*/ 5, /* Level 2*/ 3, 6, /* Level 3*/ 1, 4, EMPTY, 9, /* Level 4*/ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 8, 10});
    Node* answer = secondBiggestNodeIn(tree);
    EXPECT_EQUAL(answer->data, 9);
    freeTree(tree);
}
