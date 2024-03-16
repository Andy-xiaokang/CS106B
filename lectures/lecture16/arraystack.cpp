#include "arraystack.h"
#include "error.h"
#include <iostream>
using namespace std;

ArrayStack::ArrayStack() {
    cout << "initial stack" << endl;
    _elements = new int[CAPACITY];
    _size = 0;
    _capacity = CAPACITY;
}

ArrayStack::~ArrayStack() {
    cout << "delete stack" << endl;
    delete[] _elements;
}

void ArrayStack::push(int v) {
    if (_size == _capacity) {
        _capacity = _capacity * 2 + 1;
        cout << "capacity expand to: " << _capacity << endl;
        cout << "newArrayStack" << endl;
        int* newArrayStack = new int[_capacity];
        for (int i = 0; i < _size; i++) {
            newArrayStack[i] = _elements[i];
        }
        cout << "delete old stack" << endl;
        delete[] _elements;
        _elements = newArrayStack;
    }
    _elements[_size] = v;
    _size++;
}

int ArrayStack::pop() {
    if (isEmpty()) {
        error("the stack is empty! can't pop");
    }
    return _elements[--_size];
}

int ArrayStack::peek() const {
    if (isEmpty()) {
        error("the stack is empty! can't peek");
    }
    return _elements[_size - 1];
}

int ArrayStack::size() const {
    return _size;
}

bool ArrayStack::isEmpty() const {
    return _size == 0;
}
