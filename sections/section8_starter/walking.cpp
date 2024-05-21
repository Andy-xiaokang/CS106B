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

/* Function: contains
 * -------------------
 * Given the root of a tree and a key, returns 
 * true if the key is in the tree, and false if
 * not.
 */
bool contains(Node* root, int key) {
    // base case
    if (root == nullptr) {
        return false;
    }
    if (root->data == key) {
        return true;
    }
    // recursive case
    if (key < root->data) {
        return contains(root->left, key);
    } else {
        return contains(root->right, key);
    }
}

/* Function: insert
 * ------------------
 * Given the root of a tree and a key, inserts 
 * the key in the right position given BST 
 * ordering.
 */
void insert(Node*& root, int key) {
    // base case
    if (root == nullptr) {
        root = new Node(key, nullptr, nullptr);
        return;
    }
    if (root->data == key) {
        return;
    }
    // recursive case
    if (key > root->data) {
        insert(root->right, key);
    } else {
        insert(root->left, key);
    }
}

PROVIDED_TEST("Simple Tests for Contains") {
    Node* tree = createTreeFromVector({/* Level 1*/ 5, /* Level 2*/ 3, 6, /* Level 3*/ 1, 4, EMPTY, 9, /* Level 4*/ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, 8, 10});
    EXPECT(contains(tree, 4));
    EXPECT(!contains(tree, 11));
    freeTree(tree);
}

PROVIDED_TEST("Simple Tests for Insert") {
    Node *tree = createTreeFromVector({/* Level 1*/ 5});
    Node *bigger = createTreeFromVector({/* Level 1*/ 5, /* Level 2*/ 3, EMPTY});
    insert(tree, 3);
    EXPECT(treeEqual(tree, bigger));
    freeTree(tree);
    freeTree(bigger);
}
