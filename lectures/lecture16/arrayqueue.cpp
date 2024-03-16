#include "arrayqueue.h"
#include "error.h"

using namespace std;

ArrayQueue::ArrayQueue() {
    cout << "initial ArrayQueue" << endl;
    _elements = new int[DEFAULT_MAX_CAPACITY];
    _front = 0;
    _rear = -1;
    _maxCapacity = DEFAULT_MAX_CAPACITY;
}

ArrayQueue::~ArrayQueue() {
    cout << "delete ArrayQueue" << endl;
    delete[] _elements;
}

int ArrayQueue::dequeue() {
    if (isEmpty()) {
        error("the queue is empty! can't dequeue");
    }
    return _elements[_front++];
}

void ArrayQueue::enqueue(int value) {
    if (_rear >= _maxCapacity - 1) {
        _maxCapacity = _maxCapacity * 2 + 1;
        cout << "expand maxCapacity to " << _maxCapacity << endl;
        int* newArrayQueue = new int[_maxCapacity];
        for (int i = _front; i <= _rear; i++) {
            newArrayQueue[i] = _elements[i];
        }
        cout << "delete old ArrayQueue" << endl;
        delete[] _elements;
        _elements = newArrayQueue;
    }
    _elements[++_rear] = value;

}

bool ArrayQueue::isEmpty() const {
    if (_rear == -1) {
        return true;
    } else {
        return _front > _rear;
    }
}

int ArrayQueue::peek() const {
    if (isEmpty()) {
        error("ArrayQueue is empty can't get the peek!");
    }
    return _elements[_front];
}

int ArrayQueue::size() const {
    if (isEmpty()) {
        return 0;
    } else {
        return _rear - _front + 1;
    }
}
