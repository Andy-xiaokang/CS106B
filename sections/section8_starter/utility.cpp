#include "utility.h"
#include <string>
using namespace std;

void createTreeRec(Node*& node, Vector<int> keys, int index) {
    if (index >= keys.size()) return;
    if (keys[index] != EMPTY) {
        node = new Node(keys[index], nullptr, nullptr);
        createTreeRec(node->left, keys, 2*index + 1);
        createTreeRec(node->right, keys, 2*index + 2);
    }
}

/* This function takes in a vector representing the level-order
 * representation of a tree and turns it into a tree, returning
 * the root of the newly created tree. Empty locations in the
 * level order traversal are represented with the sentinel value
 * EMPTY.
 */
Node* createTreeFromVector(Vector<int> keys) {
    Node *root = nullptr;
    createTreeRec(root, keys, 0);
    return root;
}

void freeTree(Node* root) {
	if (root == nullptr) return;
	freeTree(root->left);
    freeTree(root->right);
	delete root;
}

bool treeEqual(Node *a, Node *b) {
    if (a == nullptr || b == nullptr) return a == b;

    return a->data == b->data
           && treeEqual(a->left, b->left)
           && treeEqual(a->right, b->right);
}

void printTree(Node* root) {
    if (root == nullptr) return;
    cout << root->data << " ";
    printTree(root->left);
    printTree(root->right);
}
