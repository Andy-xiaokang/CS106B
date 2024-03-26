#ifndef NODE_H
#define NODE_H

struct Node {
    char data;
    Node* next;
    Node(char d) {
        data = d;
        next = nullptr;
    }
};

#endif // NODE_H
