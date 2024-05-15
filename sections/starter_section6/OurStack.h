#pragma once

class OurStack {
public:
    OurStack();
    ~OurStack();

    void push(int value);
    int pop();
    int peek() const;

    int size() const;
    bool isEmpty() const;

private:
    void grow();

    int* elems;
    int logicalSize;
    int allocatedSize;
};
