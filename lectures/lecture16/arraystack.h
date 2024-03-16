#ifndef ARRAYSTACK_H
#define ARRAYSTACK_H

#define CAPACITY 3

class ArrayStack
{
public:
    ArrayStack();
    ~ArrayStack();
    void push(int v);
    int pop();
    int peek() const;
    int size() const;
    bool isEmpty() const;

private:
    int* _elements;
    int _size;
    int _capacity;
};

#endif // ARRAYSTACK_H
