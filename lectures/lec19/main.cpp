#include <iostream>
#include "console.h"

using namespace std;

struct Node {
    char data;
    Node* next;
};

Node* createNode(char data) {
    Node* n = new Node;
    n->data = data;
    n->next = nullptr;
    return n;
}

void printList(Node* head) {
    Node* curr = head;
    while( curr != nullptr) {
        cout << curr->data;
        if (curr->next != nullptr) {
            cout << "->";
        }
        curr = curr->next;
    }
    cout << endl;
}

void tailInsert(Node* & head, char data) {
    if (head == nullptr) {
        head = createNode(data);
        return;
    }
    Node* curr = head;
    while(curr->next != nullptr) {
        curr = curr->next;
    }
    Node* tail = createNode(data);
    curr->next = tail;
}

void destroyList(Node *&head) {
    if (head == nullptr) {
        return;
    }
    Node* curr = head;
    Node* next = nullptr;
    while (curr != nullptr) {
        next = curr->next;
        delete curr;
        curr = next;
    }
    head = nullptr;
}

int main() {
    Node* head = nullptr;
    tailInsert(head, 'x');
    tailInsert(head, 'i');
    // tailInsert(head, 'a');
    // tailInsert(head, 'o');
    // tailInsert(head, 'k');
    // tailInsert(head, 'a');
    // tailInsert(head, 'n');
    // tailInsert(head, 'g');
    printList(head);
    cout << "head pointer value: " << head << endl;
    destroyList(head);
    cout  << "after destory the list: head value" << head << endl;
    cout << "destroy the list and print the empty list:" << endl;
    printList(head);
    return 0;
}
