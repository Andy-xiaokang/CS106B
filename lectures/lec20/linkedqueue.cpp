#include "linkedqueue.h"
#include "error.h"

linkedqueue::linkedqueue() {
    _head = nullptr;
    _tail = nullptr;
    _size = 0;
}

linkedqueue::~linkedqueue() {
    Node* curr = nullptr;
    while(_head != nullptr) {
        curr = _head;
        _head = _head->next;
        delete curr;
    }
}

void linkedqueue::enqueue(char data) {
    if (_head == nullptr && _tail == nullptr) {
        Node* newNode = new Node(data);
        _tail = newNode;
        _head = _tail;
        _size++;
        return;
    }
    Node* newNode = new Node(data);
    _tail->next = newNode;
    _tail = newNode;
    _size++;
}


char linkedqueue::dequeue() {
    if (_head == nullptr) {
        error("can't dequeue from empty linked list");
    }
    if (_head->next == nullptr) {
        Node* tmp = _head;
        char result = tmp->data;
        _head = _head->next;
        _tail = nullptr;
        delete tmp;
        _size--;
        return result;
    }
    Node* tmp = _head;
    char result = tmp->data;
    _head = _head->next;
    delete tmp;
    _size--;
    return result;
}

char linkedqueue::peek() const {
    if (_head == nullptr) {
        error("can't get peek from empty linkedqueue");
    }
    return _head->data;
}

bool linkedqueue::isEmpty() const {
    return _size == 0;
}

int linkedqueue::size() const {
    return _size;
}


