#pragma once

#include "testing/MemoryDiagnostics.h"

struct Node {

    int data;
    Node *left;
    Node *right;

    // default constructor does not initialize
    Node() {}   

    // 3-arg constructor sets fields from arguments
    Node(int d, Node* l, Node* r) {
        data = d;
        left = l;
        right = r;
    }

   TRACK_ALLOCATIONS_OF(Node);
};
