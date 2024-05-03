#ifndef NODE_H
#define NODE_H
#include <string>
struct Node {
    std::string key;
    Node* next;
    Node(std::string data) {
        key = data;
        next = nullptr;
    }
};
#endif // NODE_H
