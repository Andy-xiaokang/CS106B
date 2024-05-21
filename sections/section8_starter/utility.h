#pragma once

#include "node.h"
#include "vector.h"

#define EMPTY INT_MAX

Node* createTreeFromVector(Vector<int> keys);
void freeTree(Node* root);
bool treeEqual(Node* a, Node *b);
void printTree(Node *root);
