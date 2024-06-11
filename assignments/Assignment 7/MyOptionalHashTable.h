#pragma once

#include "HashFunction.h"
#include "Demos/Utility.h"
#include "GUI/SimpleTest.h"
#include "GUI/MemoryDiagnostics.h"
#include <string>

/* An optional extra hash table for you to work with. You are not required to
 * implement this hash table type; it's there purely in the event that you want
 * to add extensions to the base assignment.
 *
 * There are no requirements here; you can code this up however you'd like, or
 * leave it entirely blank.
 */
class MyOptionalHashTable {
public:
    /**
     * Constructs a new linear probing table that uses the hash function given
     * as the argument. (Note that the hash function lets you determine how
     * many slots to use; you can get this by calling hashFn.numSlots().)
     */
    MyOptionalHashTable(HashFunction<std::string> hashFn);

    /**
     * Cleans up all memory allocated by this hash table.
     */
    ~MyOptionalHashTable();

    /**
     * Returns whether the table is empty.
     */
    bool isEmpty() const;

    /**
     * Returns the number of elements in the table.
     */
    int size() const;

    /**
     * Inserts the specified element into this hash table, returning whether
     * it was
     */
    bool insert(const std::string& key);

    /**
     * Returns whether the specified key is contained in this hash tasble.
     */
    bool contains(const std::string& key) const;

    /**
     * Removes the specified element from this hash table. If the element is not
     * present in the hash table, this operation is a no-op. The function then
     * returns whether anything was removed.
     */
    bool remove(const std::string& key);

    /**
     * Prints out relevant information to assist with debugging.
     */
    void printDebugInfo() const;

private:
    /* This is completely up to you. */




    /* Internal shenanigans to make this play well with C++. */
    DISALLOW_COPYING_OF(MyOptionalHashTable);
    ALLOW_TEST_ACCESS();
};
