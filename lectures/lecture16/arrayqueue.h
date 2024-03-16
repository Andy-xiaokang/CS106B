#ifndef ARRAYQUEUE_H
#define ARRAYQUEUE_H

#define DEFAULT_MAX_CAPACITY 10

class ArrayQueue
{
public:
    ArrayQueue();
    ~ArrayQueue();
    int dequeue();
    void enqueue(int value);
    bool isEmpty() const;
    int peek() const;
    int size() const;
private:
    int* _elements;
    int _front;
    int _rear;
    int _maxCapacity;
};

#endif // ARRAYQUEUE_H
