#pragma once

#include "Demos/DataPoint.h"
#include "Demos/Utility.h"
#include "GUI/SimpleTest.h"

/**
 * Priority queue type implemented using a binary heap. Refer back to the assignment handout
 * for details about how binary heaps work.
 *
 * As a reminder, you are required to do all your own memory management using new[] and
 * delete[].
 */
class HeapPQueue {
public:
    /**
     * Creates a new, empty priority queue.
     */
    HeapPQueue();

    /**
     * Cleans up all memory allocated by this priorty queue. Remember, you're responsible
     * for managing your own memory!
     */
    ~HeapPQueue();

    /**
     * Adds a new data point into the queue. This operation runs in time O(log n),
     * where n is the number of elements in the queue.
     *
     * @param data The data point to add.
     */
    void enqueue(const DataPoint& data);

    /**
     * Removes and returns the lowest-weight data point in the priority queue. If multiple
     * elements are tied for having the loweset weight, any one of them may be returned.
     *
     * If the priority queue is empty, this function calls error() to report an error.
     *
     * This operation must run in time O(log n), where n is the number of elements in the
     * queue.
     *
     * @return The lowest-weight data point in the queue.
     */
    DataPoint dequeue();

    /**
     * Returns, but does not remove, the element that would next be removed via a call to
     * dequeue.
     *
     * If the priority queue is empty, this function calls error() to report an error.
     *
     * This operation must run in time O(1).
     *
     * @return
     */
    DataPoint peek() const;

    /**
     * Returns whether the priority queue is empty.
     *
     * This operation must run in time O(1).
     *
     * @return Whether the priority queue is empty.
     */
    bool isEmpty() const;

    /**
     * Returns the number of data points in this priority queue.
     *
     * This operation must run in time O(1).
     *
     * @return The number of elements in the priority queue.
     */
    int  size() const;

    /* This function exists purely for testing purposes. You can have it do whatever you'd
     * like and we won't be invoking it when grading. In the past, students have had this
     * function print out the array representing the heap, or information about how much
     * space is allocated, etc. Feel free to use it as you see fit!
     */
    void printDebugInfo();

private:
    /* Pointer to the array of elements. Our tests expect your implementation to use the
     * name 'elems' for the pointer to the array of elements, so please do not rename this
     * variable.
     */
    DataPoint* elems = nullptr;

    /* Logical size of the priority queue (the number of elements stored in the heap).
     * Our tests expect that this variable exists, so please do not rename this variable.
     */
    int logicalSize = 0;

    /* Allocated size of the priority queue (the number of slots used in the heap).
     * Our tests expect that this variable exists, so please do not rename this variable.
     */
    int allocatedSize = 0;

    /* Constant controlling how big the original array should be.
     *
     * We've picked this value because it's large enough to hold
     * a small number of elements for when you're just getting
     * started, but small enough that it's easy to test resizing
     * behavior. You shouldn't edit this value.
     */
    static const int INITIAL_SIZE = 6;
    void bubbleUp(DataPoint* & elems);
    void bubbleDown(DataPoint* & elems);
    void resize(DataPoint* & elems);


    /* By default, C++ will let you copy objects. The problem is that the default copy
     * just does an element-by-element copy, which with pointers will give invalid results.
     * This macro disables copying of this type. For more details about how this works, and
     * for more information about how to override the default behavior, take CS106L!
     */
    DISALLOW_COPYING_OF(HeapPQueue);

    /* Grants STUDENT_TEST and PROVIDED_TEST access to the private section of this class.
     * This allows tests to check private fields to make sure they have the right values
     * and to test specific helper functions.
     */
    ALLOW_TEST_ACCESS();
};

