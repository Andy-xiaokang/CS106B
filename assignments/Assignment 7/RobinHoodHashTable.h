#pragma once

#include "HashFunction.h"
#include "Demos/Utility.h"
#include "GUI/SimpleTest.h"
#include "GUI/MemoryDiagnostics.h"
#include <string>

class RobinHoodHashTable {
public:
    /**
     * Constructs a new Robin Hood hash table that uses the hash function given
     * as the argument. (Note that the hash function lets you determine how
     * many slots to use; you can get this by calling hashFn.numSlots().)
     */
    RobinHoodHashTable(HashFunction<std::string> hashFn);

    /**
     * Cleans up all memory allocated by this hash table.
     */
    ~RobinHoodHashTable();

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
     * exists, this leaves the table unchanged. If there is no space in the table
     * to insert an element - that is, every slot is full - this should return
     * false to indicate that there is no more space.
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
     * You should implement this operation using backward-shift deletion, as
     * described in lecture.
     *
     * Returns true if the element was removed and false otherwise.
     */
    bool remove(const std::string& key);

    /**
     * Prints out relevant information to assist with debugging.
     */
    void printDebugInfo() const;

private:
    /* Type representing a slot in the table. Full slots have their distances
     * set to the distance from the home slot (measured by starting at that
     * slot and walking forward, wrapping around the table if necessary). Empty
     * slots have their distance set to the constant EMPTY_SLOT.
     *
     * Our tests expect the Slot type to look exactly like this; please do not
     * modify this type.
     */
    struct Slot {
        std::string value;
        int distance;

        TRACK_ALLOCATIONS_OF(Slot);
    };

    /* Constant used to denote that a slot is empty. Distances can't be negative,
     * so this can't be confused for a valid distance.
     */
    static const int EMPTY_SLOT = -137;

    /* Pointer to the elements. Our tests expect this field to be here, so please do
     * not change this definition.
     */
    Slot* elems = nullptr;

    HashFunction<std::string> hashCode;
    int logicalSize;
    int allocatedSize;
    int getIndex(int code) const;



    /* Internal shenanigans to make this play well with C++. */
    DISALLOW_COPYING_OF(RobinHoodHashTable);
    ALLOW_TEST_ACCESS();
    MAKE_PRINTERS_FOR(Slot);
    MAKE_COMPARATORS_FOR(Slot);
};
