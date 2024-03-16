#ifndef ARRAYVECTOR_H
#define ARRAYVECTOR_H

#define DEFAULT_CAPACITY 3

class ArrayVector
{
public:
    ArrayVector();
    ~ArrayVector();
    void add(int value);
    void insert(int index, int value);
    void remove(int index);
    void set(int index, int value);
    int get(int index) const;
    bool isEmpty() const;
    int size() const;

private:
    int* _elements;
    int _size;
    int _capacity;
};

#endif // ARRAYVECTOR_H
