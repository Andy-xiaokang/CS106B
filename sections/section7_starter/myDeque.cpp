/*
 * CS106B Section Handout Test Harness: Section 7
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified by Neel Kishnani for 
 * Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include "myDeque.h"
#include "error.h"
using namespace std;

Deque::Deque() {
    head = new Node;
    tail = new Node;
    head->next = tail;
    tail->prev = head;
    head->prev = tail->next = nullptr;
    elementNums = 0;
}

Deque::~Deque() {
    Node* curr = head;
    while (curr != nullptr) {
        Node* tmp = curr;
        curr = curr->next;
        delete tmp;
    }
    elementNums = 0;
}

Deque::Node* Deque::makeNode(int value) {
    Node* newNode = new Node;
    newNode->value = value;
    return newNode;
}

bool Deque::isEmpty() const {
    return elementNums == 0;
}

int Deque::size() const {
    return elementNums;
}

void Deque::pushFront(int value) {
    Node* newNode = makeNode(value);
    newNode->next = head->next;
    newNode->prev = head;
    head->next->prev = newNode;
    head->next = newNode;
}

void Deque::pushBack(int value) {
    Node* newNode = makeNode(value);
    newNode->next = tail;
    newNode->prev = tail->prev;
    tail->prev->next = newNode;
    tail->prev = newNode;
}

int Deque::peekFront() const {
    if (isEmpty()) {
        error("can't peekFront from empty Deque");
    }
    return head->next->value;
}

int Deque::peekBack() const {
    if (isEmpty()) {
        error("can't peekBack from empty Deque");
    }
    return tail->prev->value;
}

int Deque::popFront() {
    if (isEmpty()) {
        error("can't popFront from empty Deque");
    }
    int result = head->next->value;
    Node* tmp = head->next;
    head->next->next->prev = head;
    head->next = tmp->next;
    delete tmp;
    return result;
}

int Deque::popBack() {
    if (isEmpty()) {
        error("can't popBack from empty Deque");
    }
    int result = tail->prev->value;
    Node* tmp = tail->prev;
    tail->prev->prev->next = tail;
    tail->prev = tail->prev->prev;
    delete tmp;
    return result;
}
