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
 * Function: removeFrom
 * ---------------------
 * Given the root of a tree and a key to remove,
 * removes the key in-place.
 */
int findMax(Node* root) {
    if (root->right == nullptr) {
        return root->data;
    }
    return findMax(root->right);
}

void removeFrom(Node*& root, int value) {
    if (root == nullptr) return;
    else if (value > root->data) {
        removeFrom(root->right, value);
    } else if (value < root->data) {
        removeFrom(root->left, value);
    }
    else {
        if (root->left == nullptr && root->right == nullptr) {
            delete root;
            root = nullptr;
        } else if (root->left != nullptr && root->right == nullptr) {
            Node* tmp = root;
            root = root->left;
            delete tmp;
        } else if (root->left == nullptr && root->right != nullptr) {
            Node* tmp = root;
            root = root->right;
            delete tmp;
        } else {
            root->data = findMax(root->left);
            removeFrom(root->left, root->data);
        }
    }
}

PROVIDED_TEST("Simple tests for remove function") {
    Node* tree = createTreeFromVector({/* Level 1*/ 5, /* Level 2*/ 3, EMPTY});
    Node* modified = createTreeFromVector({ 5 });
    removeFrom(tree, 3);
    EXPECT(treeEqual(tree, modified));
    freeTree(tree);
    freeTree(modified);
}
