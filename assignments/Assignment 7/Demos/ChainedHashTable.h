#ifndef ChainedHashTable_Included
#define ChainedHashTable_Included

#include "Demos/Utility.h"
#include "HashFunction.h"
#include "vector.h"

class ChainedHashTable {
public:
    /**
     * Constructs a new chained hash table that uses the hash function given
     * as the argument.
     */
    ChainedHashTable(HashFunction<std::string> hashFn);

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
     * exists, this leaves the table unchanged.
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
     * Returns true if the element was removed and false otherwise.
     */
    bool remove(const std::string& key);


private:
    Vector<Vector<std::string>> mBuckets;
    int                         mNumElems;
    HashFunction<std::string>   mHashFn;

    DISALLOW_COPYING_OF(ChainedHashTable);
};

#endif
