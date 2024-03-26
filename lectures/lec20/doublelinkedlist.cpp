/*
#include <iostream>
#include "error.h"

using namespace std;

struct Node {
    char data;
    Node* prev;
    Node* next;
    Node(char d) {
        data = d;
        prev = nullptr;
        next = nullptr;
    }
};

void headInsert(Node* & head, Node* & tail, char data) {
    if (head == nullptr) {
        head = new Node(data);
        tail = head;
        head->next = nullptr;
        head->prev = nullptr;
        return;
    }
    Node* newNode = new Node(data);
    newNode->next = head;
    head->prev = newNode;
    head = newNode;
}

void tailInsert(Node* & head, Node* & tail, char data) {
    if (tail == nullptr) {
        tail = new Node(data);
        head = tail;
        tail->next = nullptr;
        tail->prev = nullptr;
        return;
    }
    Node* newNode = new Node(data);
    tail->next = newNode;
    newNode->prev = tail;
    tail = newNode;
}

char headDelete(Node* & head, Node* & tail) {
    if (head == nullptr) {
        error("can't head delete in empty list");
    }
    Node* tmp = head;
    char result = tmp->data;
    head = head->next;
    delete tmp;
    if (head == nullptr) {
        tail = nullptr;
        return result;
    }
    head->prev = nullptr;
    return result;
}

char tailDelete(Node* & head, Node* & tail) {
    if (tail == nullptr) {
        error("can't, tail delete in empty list");
    }
    Node* tmp = tail;
    char result = tail->data;
    if (head->next == nullptr) {
        head = nullptr;
        tail = nullptr;
        delete tmp;
        return result;
    }
    tail = tail->prev;
    tail->next = nullptr;
    delete tmp;
    return result;
}

int main() {
    Node* head = nullptr;
    Node* tail = nullptr;
    // headInsert(head, tail, 'x');
    // headInsert(head, tail, 'i');
    // headInsert(head, tail, 'a');
    // headInsert(head, tail, 'o');
    tailInsert(head, tail, 'x');
    tailInsert(head, tail, 'i');
    tailInsert(head, tail, 'a');
    tailInsert(head, tail, 'o');
    while( head != nullptr) {
        // cout << headDelete(head, tail);
        cout << tailDelete(head, tail);
    }
    cout << endl;
    cout << "head: " << head << endl;
    cout << "tail: " << tail << endl;
    return 0;
}
*/
