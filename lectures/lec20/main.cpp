#include <iostream>
#include "linkedqueue.h"
using namespace std;

int main() {
    linkedqueue queue;
    cout << sizeof(linkedqueue) << endl;
    // cout << &queue << &queue._head << &queue._tail << &queue._size << endl;
    queue.enqueue('x');
    queue.enqueue('i');
    queue.enqueue('a');
    queue.enqueue('o');
    cout << "queue size: " << queue.size() << endl;
    cout << "is empty: " << queue.isEmpty() << endl;
    cout << "queue's peek: " << queue.peek() << endl;
    while (!queue.isEmpty()) {
        cout << queue.dequeue() << endl;
    }
    cout << "is empty: " << queue.isEmpty() << endl;
    cout << "queue's size: " << queue.size() << endl;
    // cout << "queue's peek: " << queue.peek() << endl;
    return 0;
}

/*
#include <iostream>
#include "console.h"
#include "error.h"

using namespace std;

struct Node
{
    char data;
    Node* next;
    Node(char d) {
        data = d;
        next = nullptr;
    }
};

void headInsert(Node* & head, Node* & tail, char data) {
    Node* n = new Node(data);
    n->next = head;
    head = n;
    if (tail == nullptr) {
        tail = head;
    }
}

char popHead(Node* & head, Node* & tail) {
    if (head == nullptr) {
        error("can't pop from empty list");
    }
    Node* tmp = head;
    char result = tmp->data;
    head = head->next;
    delete tmp;
    if (head == nullptr) {
        tail = nullptr;
    }
    return result;
}

char tailDelete(Node* & head, Node* & tail) {
    if (tail == nullptr) {
        error("can't delete tail from a empty list");
    }
    Node* tmp = tail;
    char result = tmp->data;
    if (head->next == nullptr) {
        head = nullptr;
        tail = nullptr;
        delete tmp;
        return result;
    }
    Node* curr = head;
    while (curr->next != tail) {
        curr = curr->next;
    }
    tail = curr;
    tail->next = nullptr;
    delete tmp;
    return result;
}

int main() {
    Node* head = nullptr;
    Node* tail = nullptr;
    headInsert(head, tail, 'x');
    headInsert(head, tail, 'i');
    headInsert(head, tail, 'a');
    headInsert(head, tail, 'o');
    while (head != nullptr) {
        // cout << popHead(head, tail);
        cout << tailDelete(head, tail);
    }
    cout << endl;
    cout << "head: " << head << endl;
    cout << "tail: " << tail << endl;
    return 0;
}
*/
