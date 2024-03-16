#include "arrayvector.h"
#include "error.h"
#include <iostream>
using namespace std;

ArrayVector::ArrayVector() {
    cout << "intial Vector" << endl;
    _elements = new int[DEFAULT_CAPACITY];
    _size = 0;
    _capacity = DEFAULT_CAPACITY;
}

ArrayVector::~ArrayVector() {
    cout << "delete Vector" << endl;
    delete[] _elements;
}

void ArrayVector::add(int value) {
    if (_size >= _capacity) {
        _capacity = _capacity * 2 + 1;
        cout << "expand capacity to " << _capacity << endl;
        int* newArrayVector = new int[_capacity];
        for (int i = 0; i < _size; i++) {
            newArrayVector[i] = _elements[i];
        }
        cout << "delete old small ArrayVector" << endl;
        delete _elements;
        _elements = newArrayVector;
    }
    _elements[_size++] = value;
}

void ArrayVector::insert(int index, int value) {
    if (index < 0 || index >= _size) {
        error("index out of range! can't insert");
    }
    if (_size >= _capacity) {
        _capacity = _capacity * 2 + 1;
        cout << "expand capacity to " << _capacity << endl;
        int* newArrayVector = new int[_capacity];
        for (int i = 0; i < _size; i++) {
            newArrayVector[i] = _elements[i];
        }
        cout << "delete old small ArrayVector" << endl;
        delete _elements;
        _elements = newArrayVector;
    }
    for (int i = _size -1; i >= index; i--) {
        _elements[i+1] = _elements[i];
    }
    _elements[index] = value;
    _size++;
}

void ArrayVector::remove(int index) {
    if (index < 0 || index >= _size) {
        error("index out of range, can't remove");
    }
    for (int i = index + 1; i < _size; i++) {
        _elements[i-1] = _elements[i];
    }
    _size--;
}

void ArrayVector::set(int index, int value) {
    if (index < 0 || index >= _size) {
        error("index out of range! can't set");
    }
    _elements[index] = value;
}

int ArrayVector::get(int index) const {
    if (index < 0 || index >= _size) {
        error("index out of range! can't get element");
    }
    return _elements[index];
}

bool ArrayVector::isEmpty() const {
    return _size == 0;
}

int ArrayVector::size() const {
    return _size;
}
