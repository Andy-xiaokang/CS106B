#pragma once

#include "HashFunction.h"
#include "Demos/Utility.h"
#include "GUI/SimpleTest.h"
#include "GUI/MemoryDiagnostics.h"
#include <string>

class LinearProbingHashTable {
public:
    /**
     * Constructs a new linear probing table that uses the hash function given
     * as the argument. (Note that the hash function lets you determine how
     * many slots to use; you can get this by calling hashFn.numSlots().)
     */
    LinearProbingHashTable(HashFunction<std::string> hashFn);

    /**
     * Cleans up all memory allocated by this hash table.
     */
    ~LinearProbingHashTable();

    /**
     * Returns whether the table is empty.
     */
    bool isEmpty() const;

    /**
     * Returns the number of elements in the table.
     */
    int size() const;

    /**
     * Inserts the specified element into this hash table. If the element already
     * exists, this leaves the table unchanged and returns false to indicate that
     * nothing was added.
     *
     * If there is no space in the table to insert an element - that is, every slot
     * is full - this should return false to indicate that there is no more space.
     *
     * This function returns whether the element was inserted into the table.
     */
    bool insert(const std::string& key);

    /**
     * Returns whether the specified key is contained in this hash tasble.
     */
    bool contains(const std::string& key) const;

    /**
     * Removes the specified element from this hash table. If the element is not
     * present in the hash table, this operation is a no-op.
     *
     * You should implement this operation using tombstone deletion - mark the
     * slot containing the element as a tombstone.
     *
     * Returns true if the element was removed and false otherwise.
     */
    bool remove(const std::string& key);

    /**
     * Prints out relevant information to assist with debugging.
     */
    void printDebugInfo() const;

private:
    /* Type representing the state of a given slot. Please do not modify
     * this type, as our tests expect it to look exactly as it does
     * right now.
     */
    enum class SlotType {
        EMPTY, FILLED, TOMBSTONE
    };

    /* Type representing a slot in a linear probing table. Please do not
     * modify this type, as our tests expect it to look exactly as it does
     * right now.
     */
    struct Slot {
        std::string value;
        SlotType type;

        TRACK_ALLOCATIONS_OF(Slot);
    };

    /* Pointer to the array of elements. Our tests expect your implementation to use the
     * name 'elems' for the pointer to the array of elements, so please do not rename this
     * variable.
     */
    Slot* elems = nullptr;

    int logicalSize;
    int allocatedSize;
    HashFunction<std::string> hashCode;


    /* Internal shenanigans to make this play well with C++. */
    DISALLOW_COPYING_OF(LinearProbingHashTable);
    ALLOW_TEST_ACCESS();
    MAKE_PRINTERS_FOR(SlotType);
    MAKE_PRINTERS_FOR(Slot);
    MAKE_COMPARATORS_FOR(Slot);
};
