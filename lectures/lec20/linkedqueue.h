#ifndef LINKEDQUEUE_H
#define LINKEDQUEUE_H
#include "node.h"

class linkedqueue
{
public:
    linkedqueue();
    ~linkedqueue();
    void enqueue(char data);
    char dequeue();
    char peek() const;
    bool isEmpty() const;
    int size() const;
private:
    Node* _head;
    Node* _tail;
    int _size;
};

#endif // LINKEDQUEUE_H
