#include "RobinHoodHashTable.h"
#include <algorithm>
using namespace std;

RobinHoodHashTable::RobinHoodHashTable(HashFunction<string> hashFn) {
    this->hashCode = hashFn;
    logicalSize = 0;
    allocatedSize = hashCode.numSlots();
    elems = new Slot[allocatedSize];
    for (int i = 0; i < allocatedSize; i++) {
        elems[i].distance = EMPTY_SLOT;
    }
}

RobinHoodHashTable::~RobinHoodHashTable() {
    delete[] elems;
}

int RobinHoodHashTable::size() const {
    return logicalSize;
}

bool RobinHoodHashTable::isEmpty() const {
    return logicalSize == 0;
}

int RobinHoodHashTable::getIndex(int code) const{
    return code % allocatedSize;
}

bool RobinHoodHashTable::insert(const string& elem) {
    if (contains(elem) || this->size() == allocatedSize) return false;
    int code = hashCode(elem);
    Slot target = {elem, 0};
    while (elems[getIndex(code)].distance != EMPTY_SLOT) {
        if (target.distance > elems[getIndex(code)].distance) {
            swap(target, elems[getIndex(code)]);
        }
        code++;
        target.distance++;
    }
    elems[getIndex(code)] = target;
    logicalSize++;
    return true;
}

bool RobinHoodHashTable::contains(const string& elem) const {
    int code = hashCode(elem);
    Slot target = {elem, 0};
    while (elems[getIndex(code)].distance != EMPTY_SLOT && target.distance <= elems[getIndex(code)].distance) {
        if (elems[getIndex(code)].value == target.value) {
            return true;
        } else {
            code++;
            target.distance++;
        }
    }
    return false;
}

bool RobinHoodHashTable::remove(const string& elem) {
    if (!contains(elem)) return false;
    int code = hashCode(elem);
    while (elems[getIndex(code)].distance != EMPTY_SLOT) {
        if (elems[getIndex(code)].value == elem) {
            elems[getIndex(code)].distance = EMPTY_SLOT;
            while (elems[getIndex(code+1)].distance != EMPTY_SLOT && elems[getIndex(code+1)].distance != 0) {
                elems[getIndex(code+1)].distance--;
                swap(elems[getIndex(code)], elems[getIndex(code+1)]);
                code++;
            }
            logicalSize--;
            return true;
        }
        code++;
    }
    return false;
}

void RobinHoodHashTable::printDebugInfo() const {
    /* TODO: Remove this comment and implement this function. */
}




/* * * * * * Test Cases Below This Point * * * * * */
#include "GUI/SimpleTest.h"

/* Optional: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */
#include "Demos/Utility.h"
#include "vector.h"

PROVIDED_TEST("Table is initially empty.") {
    RobinHoodHashTable table(Hash::random(10));

    EXPECT_EQUAL(table.size(), 0);
    EXPECT(table.isEmpty());

    /* Check table internals. */
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }
}

PROVIDED_TEST("Can insert and look up a single value.") {
    RobinHoodHashTable table(Hash::identity(10));

    EXPECT(!table.contains("0"));
    EXPECT(table.insert("0"));
    EXPECT(table.contains("0"));

    /* Check table internals. */
    EXPECT_EQUAL(table.elems[0], { "0", 0 });
    for (int i = 1; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }
}

PROVIDED_TEST("Is case-sensitive.") {
    RobinHoodHashTable table(Hash::zero(10));

    EXPECT(!table.contains("a"));
    EXPECT(!table.contains("A"));
    EXPECT(table.insert("a"));

    /* Check table internals. */
    EXPECT_EQUAL(table.elems[0], { "a", 0 });
    for (int i = 1; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }

    EXPECT(table.contains("a"));
    EXPECT(!table.contains("A"));
}

PROVIDED_TEST("Insertions work with hash collisions.") {
    /* Use a very, very bad hash function that maps everything to slot zero. */
    RobinHoodHashTable table(Hash::zero(10));

    Vector<string> toAdd = {
        "Quokka", "Pudu", "Gerenuk", "Dikdik"
    };
    Vector<string> toNotAdd = {
        "Springbok", "Kudu"
    };

    for (string animal: toAdd) {
        EXPECT(table.insert(animal));
    }
    EXPECT_EQUAL(table.size(), toAdd.size());

    /* Validate internals. The first table should look like this:
     *
     * Quokka Pudu Gerenuk Dikdik (empty) (empty) (empty) (empty) (empty) (empty)
     *   0      1     2      3
     */
    for (int i = 0; i < 10; i++) {
        if (i < toAdd.size()) {
            EXPECT_EQUAL(table.elems[i], { toAdd[i], i });
        } else {
            EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
        }
    }

    for (string animal: toAdd) {
        EXPECT(table.contains(animal));
    }
    for (string animal: toNotAdd) {
        EXPECT(!table.contains(animal));
    }
}

PROVIDED_TEST("Insertions don't displace items at same distance from home.") {
    /* The hash function we use maps strings to their numeric values. This allows
     * us to control the contents of the hash table.
     */
    RobinHoodHashTable table(Hash::identity(10));

    /* Place 0 and 10 in. The table should now look like this:
     *
     *            0 10 . . . . . . . .
     *            0  1
     *
     */
    EXPECT(table.insert("0"));
    EXPECT(table.insert("10"));

    EXPECT_EQUAL(table.elems[0], {  "0", 0 });
    EXPECT_EQUAL(table.elems[1], { "10", 1 });

    for (int i = 2; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }

    EXPECT(table.contains("0"));
    EXPECT(table.contains("10"));

    /* Now, insert the values 1, 2, 3, 4, and 5. The table should look
     * like this:
     *
     *           0 10  1  2  3  4  5  .  .  .
     *           0  1  1  1  1  1  1
     */
    EXPECT(table.insert("1"));
    EXPECT(table.insert("2"));
    EXPECT(table.insert("3"));
    EXPECT(table.insert("4"));
    EXPECT(table.insert("5"));

    EXPECT_EQUAL(table.elems[0], {  "0", 0 });
    EXPECT_EQUAL(table.elems[1], { "10", 1 });
    EXPECT_EQUAL(table.elems[2], {  "1", 1 });
    EXPECT_EQUAL(table.elems[3], {  "2", 1 });
    EXPECT_EQUAL(table.elems[4], {  "3", 1 });
    EXPECT_EQUAL(table.elems[5], {  "4", 1 });
    EXPECT_EQUAL(table.elems[6], {  "5", 1 });
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);

    /* Make sure we can find everything. */
    EXPECT(table.contains("0"));
    EXPECT(table.contains("1"));
    EXPECT(table.contains("2"));
    EXPECT(table.contains("3"));
    EXPECT(table.contains("4"));
    EXPECT(table.contains("5"));
    EXPECT(!table.contains("6"));
    EXPECT(!table.contains("7"));
    EXPECT(!table.contains("8"));
    EXPECT(!table.contains("9"));
    EXPECT(table.contains("10"));

    EXPECT_EQUAL(table.size(), 7);
}

PROVIDED_TEST("Insertions displace elements that are closer to home.") {
    /* The hash function we use maps strings to their numeric values. This allows
     * us to control the contents of the hash table.
     */
    RobinHoodHashTable table(Hash::identity(10));

    /* Insert a sequence of elements that gives back this table:
     *
     *   .  1  .  3  .  .  6  .  .  .
     *      0     0        0
     */
    EXPECT(table.insert("1"));
    EXPECT(table.insert("3"));
    EXPECT(table.insert("6"));

    EXPECT_EQUAL(table.elems[0].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[1], {  "1", 0 });
    EXPECT_EQUAL(table.elems[2].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[3], {  "3", 0 });
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6], {  "6", 0 });
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);

    EXPECT_EQUAL(table.size(), 3);

    /* Insert 13 and 23, giving this table:
     *
     *   .  1  .  3 13 23  6  .  .  .
     *      0     0  1  2  0
     */
    EXPECT(table.insert("13"));
    EXPECT(table.insert("23"));

    EXPECT_EQUAL(table.elems[0].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[1], {  "1", 0 });
    EXPECT_EQUAL(table.elems[2].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[3], {  "3", 0 });
    EXPECT_EQUAL(table.elems[4], { "13", 1 });
    EXPECT_EQUAL(table.elems[5], { "23", 2 });
    EXPECT_EQUAL(table.elems[6], {  "6", 0 });
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);

    EXPECT_EQUAL(table.size(), 5);

    /* Insert 5, giving this table:
     *
     *   .  1  .  3 13 23  5  6  .  .
     *      0     0  1  2  1  1
     */
    EXPECT(table.insert("5"));

    EXPECT_EQUAL(table.elems[0].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[1], {  "1", 0 });
    EXPECT_EQUAL(table.elems[2].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[3], {  "3", 0 });
    EXPECT_EQUAL(table.elems[4], { "13", 1 });
    EXPECT_EQUAL(table.elems[5], { "23", 2 });
    EXPECT_EQUAL(table.elems[6], {  "5", 1 });
    EXPECT_EQUAL(table.elems[7], {  "6", 1 });
    EXPECT_EQUAL(table.elems[8].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);

    EXPECT_EQUAL(table.size(), 6);

    /* Insert 11, giving this table:
     *
     *   .  1 11  3 13 23  5  6  .  .
     *      0  1  0  1  2  1  1
     */
    EXPECT(table.insert("11"));

    EXPECT_EQUAL(table.elems[0].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[1], {  "1", 0 });
    EXPECT_EQUAL(table.elems[2], { "11", 1 });
    EXPECT_EQUAL(table.elems[3], {  "3", 0 });
    EXPECT_EQUAL(table.elems[4], { "13", 1 });
    EXPECT_EQUAL(table.elems[5], { "23", 2 });
    EXPECT_EQUAL(table.elems[6], {  "5", 1 });
    EXPECT_EQUAL(table.elems[7], {  "6", 1 });
    EXPECT_EQUAL(table.elems[8].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);

    EXPECT_EQUAL(table.size(), 7);

    /* Insert 21. This triggers a chain of displacements that ends like this:
     *
     *   .  1 11  21 13 23  3  5  6  .
     *      0  1  2   1  2  3  2  2
     *
     *  Why does 3 end up after 13 and 23? It's because when 3 is displaced
     *  initially, it's one spot away from home, and the next slot (13) is
     *  also one slot from home. We have a rule of never displacing an element
     *  in the table unless it is strictly closer to home than us, and in this
     *  case we're tied. Then, when comparing 3 against the next slot (23), we
     *  similarly have a tie (both are at distance 2), so we don't swap it in.
     *  However, 3 does fit into the next slot.
     */
    EXPECT(table.insert("21"));

    EXPECT_EQUAL(table.elems[0].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[1], {  "1", 0 });
    EXPECT_EQUAL(table.elems[2], { "11", 1 });
    EXPECT_EQUAL(table.elems[3], { "21", 2 });
    EXPECT_EQUAL(table.elems[4], { "13", 1 });
    EXPECT_EQUAL(table.elems[5], { "23", 2 });
    EXPECT_EQUAL(table.elems[6], {  "3", 3 });
    EXPECT_EQUAL(table.elems[7], {  "5", 2 });
    EXPECT_EQUAL(table.elems[8], {  "6", 2 });
    EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);

    EXPECT_EQUAL(table.size(), 8);

    /* Insert 0 and 10. This triggers a chain of displacements that ends like this:
     *
     *   0 10 11 21  1 23  3 13  5  6
     *   0  1  1  2  3  2  3  4  3  3
     *
     *  Notice that 1 has leapfrogged to the end of the range of things hashing to
     *  slot 1 and that 13 has leapfrogged to the end of its range as well.
     */
    EXPECT(table.insert("0"));
    EXPECT(table.insert("10"));

    EXPECT_EQUAL(table.elems[0], {  "0", 0 });
    EXPECT_EQUAL(table.elems[1], { "10", 1 });
    EXPECT_EQUAL(table.elems[2], { "11", 1 });
    EXPECT_EQUAL(table.elems[3], { "21", 2 });
    EXPECT_EQUAL(table.elems[4], {  "1", 3 });
    EXPECT_EQUAL(table.elems[5], { "23", 2 });
    EXPECT_EQUAL(table.elems[6], {  "3", 3 });
    EXPECT_EQUAL(table.elems[7], { "13", 4 });
    EXPECT_EQUAL(table.elems[8], {  "5", 3 });
    EXPECT_EQUAL(table.elems[9], {  "6", 3 });

    EXPECT_EQUAL(table.size(), 10);

}

PROVIDED_TEST("Wraps around the end of the table.") {
    /* Everything goes in slot 7. This is a terrible hash function that's just used for
     * testing purposes.
     */
    RobinHoodHashTable table(Hash::constant(10, 7));

    /* Insert a bunch of values. */
    Vector<string> toAdd = {
        "H", "He", "Li", "Be", "B", "C", "N"
    };
    Vector<string> toNotAdd = {
        "O", "F", "Ne"
    };

    /* Add the elements in. */
    for (string elem: toAdd) {
        EXPECT(table.insert(elem));
    }

    /* The table should look like this:
     *
     *   Be B  C  N  .  .  .  H He Li
     *   3  4  5  6           0  1  2
     */
    EXPECT_EQUAL(table.elems[0], { "Be", 3 });
    EXPECT_EQUAL(table.elems[1], { "B",  4 });
    EXPECT_EQUAL(table.elems[2], { "C",  5 });
    EXPECT_EQUAL(table.elems[3], { "N",  6 });
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7], { "H",  0 });
    EXPECT_EQUAL(table.elems[8], { "He", 1 });
    EXPECT_EQUAL(table.elems[9], { "Li", 2 });

    /* Confirm they're all there. */
    for (string elem: toAdd) {
        EXPECT(table.contains(elem));
    }

    /* Confirm others aren't. */
    for (string elem: toNotAdd) {
        EXPECT(!table.contains(elem));
    }
}

PROVIDED_TEST("Displacements wrap around the end of the table.") {
    /* Each number hashes to its last digit. */
    RobinHoodHashTable table(Hash::identity(10));

    /* Insert some initial values to get this table:
     *
     *     0  1  .  .  .  .  .  .  8  9
     *     0  0                    0  0
     */
    EXPECT(table.insert("8"));
    EXPECT(table.insert("9"));
    EXPECT(table.insert("0"));
    EXPECT(table.insert("1"));
    EXPECT_EQUAL(table.size(), 4);

    EXPECT_EQUAL(table.elems[0], { "0",  0 });
    EXPECT_EQUAL(table.elems[1], { "1",  0 });
    EXPECT_EQUAL(table.elems[2].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[3].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8], { "8",  0 });
    EXPECT_EQUAL(table.elems[9], { "9",  0 });

    /* Insert 19. This should wrap around to give the following:
     *
     *     19 0  1  .  .  .  .  .  8  9
     *     1  1  1                 0  0
     */
    EXPECT(table.insert("19"));
    EXPECT_EQUAL(table.size(), 5);

    EXPECT_EQUAL(table.elems[0], { "19", 1 });
    EXPECT_EQUAL(table.elems[1], { "0",  1 });
    EXPECT_EQUAL(table.elems[2], { "1",  1 });
    EXPECT_EQUAL(table.elems[3].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8], { "8",  0 });
    EXPECT_EQUAL(table.elems[9], { "9",  0 });

    /* Insert 18. This should wrap around to give the following:
     *
     *     19 9  0  1  .  .  .  .  8  18
     *     1  2  2  2              0  1
     *
     * Why should the 9 now appear after the 19? Because we only displace an
     * element if it is strictly closer to home than us. Both 9 and 19 are
     * tied for having the same distance from home, so when we compare the 9
     * and the 19 during displacements, we should leave the 19 in place.
     */
    EXPECT(table.insert("18"));
    EXPECT_EQUAL(table.size(), 6);

    EXPECT_EQUAL(table.elems[0], { "19", 1 });
    EXPECT_EQUAL(table.elems[1], { "9",  2 });
    EXPECT_EQUAL(table.elems[2], { "0",  2 });
    EXPECT_EQUAL(table.elems[3], { "1",  2 });
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8], { "8",  0 });
    EXPECT_EQUAL(table.elems[9], { "18", 1 });

    /* Insert 10. This should give the following:
     *
     *     19 9  0 10  1  .  .  .  8  18
     *     1  2  2  3  3           0  1
     */
    EXPECT(table.insert("10"));
    EXPECT_EQUAL(table.size(), 7);

    EXPECT_EQUAL(table.elems[0], { "19", 1 });
    EXPECT_EQUAL(table.elems[1], {  "9", 2 });
    EXPECT_EQUAL(table.elems[2], {  "0", 2 });
    EXPECT_EQUAL(table.elems[3], { "10", 3 });
    EXPECT_EQUAL(table.elems[4], {  "1", 3 });
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8], { "8",  0 });
    EXPECT_EQUAL(table.elems[9], { "18", 1 });

    /* Insert 6, then 16, then 26. This does a series of wraparound displacements
     * that give rise to the following table:
     *
     *     8  9 19 10  0  1  6 16  26 18
     *     2  2  3  3  4  4  0  1  2  1
     *
     * Notice how each of the ranges of equal hashes has had the first element "leapfrog"
     * over to the back of the range.
     */
    EXPECT(table.insert("6"));
    EXPECT(table.insert("16"));
    EXPECT(table.insert("26"));

    EXPECT_EQUAL(table.elems[0], {  "8", 2 });
    EXPECT_EQUAL(table.elems[1], {  "9", 2 });
    EXPECT_EQUAL(table.elems[2], { "19", 3 });
    EXPECT_EQUAL(table.elems[3], { "10", 3 });
    EXPECT_EQUAL(table.elems[4], {  "0", 4 });
    EXPECT_EQUAL(table.elems[5], {  "1", 4 });
    EXPECT_EQUAL(table.elems[6], {  "6", 0 });
    EXPECT_EQUAL(table.elems[7], { "16", 1 });
    EXPECT_EQUAL(table.elems[8], { "26", 2 });
    EXPECT_EQUAL(table.elems[9], { "18", 1 });

    EXPECT_EQUAL(table.size(), 10);

    EXPECT(table.contains("8"));
    EXPECT(table.contains("9"));
    EXPECT(table.contains("0"));
    EXPECT(table.contains("1"));
    EXPECT(table.contains("19"));
    EXPECT(table.contains("18"));
    EXPECT(table.contains("10"));
    EXPECT(table.contains("6"));
    EXPECT(table.contains("16"));
    EXPECT(table.contains("26"));
}

PROVIDED_TEST("Doesn't allow for duplicates.") {
    /* Drop everything into slot zero, just for consistency. */
    RobinHoodHashTable table(Hash::zero(10));

    EXPECT(table.insert("dikdik"));
    EXPECT_EQUAL(table.size(), 1);

    EXPECT_EQUAL(table.elems[0], { "dikdik", 0 });
    for (int i = 1; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }

    /* Insert the same value more times than the table can hold. */
    for (int i = 0; i < 100; i++) {
        EXPECT(!table.insert("dikdik"));
        EXPECT_EQUAL(table.size(), 1);

        EXPECT_EQUAL(table.elems[0], { "dikdik", 0 });
        for (int i = 1; i < 10; i++) {
            EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
        }
    }
}

PROVIDED_TEST("Handles inserting the empty string.") {
    RobinHoodHashTable table(Hash::zero(10));

    EXPECT(!table.contains(""));
    EXPECT(table.insert(""));

    EXPECT_EQUAL(table.elems[0], { "", 0 });
    for (int i = 1; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }

    EXPECT(table.contains(""));
    EXPECT_EQUAL(table.size(), 1);
}

PROVIDED_TEST("Lookups work even if the table is full.") {
    /* Dump everything in bucket 7. This is a terrible hash function, but it's
     * useful for testing.
     */
    RobinHoodHashTable table(Hash::constant(10, 7));

    /* Fill the table. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.insert(to_string(i)));
    }
    EXPECT_EQUAL(table.size(), 10);

    /* Validate the table. */
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(table.elems[(i + 7) % 10], { to_string(i), i });
    }

    /* Search for all present items. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.contains(to_string(i)));
    }

    /* Confirm elements that aren't there don't show up. Watch out! This edge
     * case might cause your code to hang if you haven't anticipated it.
     */
    for (int i = 10; i < 20; i++) {
        EXPECT(!table.contains(to_string(i)));
    }
}

PROVIDED_TEST("Won't insert elements if table is full.") {
    /* Terrible hash function that places everything in slot seven. */
    RobinHoodHashTable table(Hash::constant(10, 7));

    /* Load the table. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.insert(to_string(i))); // Should succeed
    }
    EXPECT_EQUAL(table.size(), 10);

    /* Validate the table. */
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(table.elems[(i + 7) % 10], { to_string(i), i });
    }

    /* All these operations should fail. */
    for (int i = 10; i < 20; i++) {
        EXPECT(!table.insert(to_string(i)));
    }
}

PROVIDED_TEST("Stress Test: Searches cut off early (should take at most five seconds)") {
    /* We're going to fill an enormous table with a bunch of blocks of elements, then try doing
     * lookups. Using regular linear probing, this will grind to a halt because the table is
     * full. Using Robin Hood hashing, this should cut off extremely quickly.
     *
     * The table will generally look like this:
     *
     * |--- block with hash code 0 ---| |-- block with hash code 1 ---| |-- block with hash code 2 --| ...
     *
     * If we ever do a lookup for an element, it should never run off the end of a block, because
     * as soon as we hit the start of the next block the element we're scanning for will be
     * further from home than the element at the block start.
     */
    const int kElemsPerBlock = 5;
    const int kNumBlocks     = 50000;
    const int kNumElems      = kNumBlocks * kElemsPerBlock;

    /* Each element's position is given by its value, mod the table size. */
    RobinHoodHashTable table(Hash::identity(kNumElems));

    /* Fill the table with many blocks of values with the same hash code. */
    for (int block = 0; block < kNumBlocks; block++) {
        for (int elem = 0; elem < kElemsPerBlock; elem++) {
            /* These values should all drop into a position given by the block index
             * times the number of blocks per element.
             */
            int value = block * kElemsPerBlock + elem * kNumElems;
            EXPECT(table.insert(to_string(value)));
        }
    }
    EXPECT_EQUAL(table.size(), kNumElems);

    /* Validate the table. */
    for (int block = 0; block < kNumBlocks; block++) {
        for (int elem = 0; elem < kElemsPerBlock; elem++) {
            int value = block * kElemsPerBlock + elem * kNumElems;
            int index = block * kElemsPerBlock + elem;
            EXPECT_EQUAL(table.elems[index], { to_string(value), elem });
        }
    }

    /* Confirm each element is present. This should be very quick since each block is
     * small and all elements within a block are close to the block start.
     */
    for (int block = 0; block < kNumBlocks; block++) {
        for (int elem = 0; elem < kElemsPerBlock; elem++) {
            /* These values should all drop into a position given by the block index
             * times the number of blocks per element.
             */
            int value = block * kElemsPerBlock + elem * kNumElems;
            EXPECT(table.contains(to_string(value)));
        }
    }

    /* Now, try looking up a bunch of values that aren't there. Each search for
     * a value should terminate quickly because as soon as we scan past the end of
     * a block, the distance value of the currently-scanned element will reset to
     * zero and the search can stop.
     *
     * On the other hand, if your search procedure does the standard linear probing
     * approach of scanning until a free slot is found, then you'll have to scan all
     * positions in the table once for each position in the table, which will take
     * far too long to finish in any reasonable amount of time.
     */
    for (int block = 0; block < kNumBlocks; block++) {
        for (int elem = kElemsPerBlock; elem < 2 * kElemsPerBlock; elem++) {
            /* These values should all drop into a position given by the block index
             * times the number of blocks per element.
             */
            int value = block * kElemsPerBlock + elem * kNumElems;
            EXPECT(!table.contains(to_string(value)));
        }
    }
}

PROVIDED_TEST("Stress Test: Handles pure insertion of elements (should take at most three seconds).") {
    const int kNumTrials = 50; // Do this lots of times to smoke out any errors that might be lurking.
    for (int trial = 0; trial < kNumTrials; trial++) {
        RobinHoodHashTable table(Hash::random(100));

        const int kNumElems = 75;
        for (int i = 0; i < kNumElems; i++) {
            /* Confirm only the proper elements exist at this point. */
            for (int j = 0; j < kNumElems; j++) {
                EXPECT_EQUAL(table.contains(to_string(j)), j < i);
            }

            /* Add the element. */
            EXPECT(table.insert(to_string(i))); // Should succeed
            EXPECT_EQUAL(table.size(), i + 1);
        }
    }
}

PROVIDED_TEST("Stress Test: Inserts/searches work in expected time O(1) (should take at most three seconds).") {
    /* Huge number of slots. */
    const int kNumSlots = 1000000;

    /* Create an enormous hash table with a random hash function. */
    RobinHoodHashTable table(Hash::random(kNumSlots));

    /* Search the table for lots of elements. This should quick, since
     * the table is empty.
     */
    for (int i = 0; i < kNumSlots; i++) {
        EXPECT(!table.contains(to_string(i)));
    }

    /* Insert a lot elements. */
    const int kLotsOfElems = 100000; // 10% load factor - quite small!
    for (int i = 0; i < kLotsOfElems; i++) {
        EXPECT(table.insert(to_string(i)));
    }
    for (int i = 0; i < kLotsOfElems; i++) {
        EXPECT(table.contains(to_string(i)));
    }

    /* Confirm other elements aren't there. These false lookups should still be fast
     * due to the low load factor.
     */
    for (int i = kLotsOfElems; i < 2 * kLotsOfElems; i++) {
        EXPECT(!table.contains(to_string(i)));
    }
}

PROVIDED_TEST("Can insert and remove a single element.") {
    /* Everything hashes to zero. This is a *terrible* hash function and is used
     * purely for testing.
     */
    RobinHoodHashTable table(Hash::zero(200));

    /* Insert an element. */
    EXPECT(table.insert("137"));
    EXPECT_EQUAL(table.size(), 1);
    EXPECT(!table.isEmpty());

    /* Validate table internals. */
    EXPECT_EQUAL(table.elems[0], { "137", 0 });
    for (int i = 1; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }

    /* Remove the element. */
    EXPECT(table.remove("137"));
    EXPECT_EQUAL(table.size(), 0);
    EXPECT(table.isEmpty());

    /* Validate table internals. */
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }

    EXPECT(!table.contains("137"));
}

PROVIDED_TEST("Handles removing the empty string.") {
    RobinHoodHashTable table(Hash::zero(10));

    /* We shouldn't be able to remove the empty string from the table;
     * it's not there.
     */
    EXPECT(!table.contains(""));
    EXPECT(!table.remove(""));

    /* Add the empty string. */
    EXPECT(table.insert(""));

    /* Validate table internals. */
    EXPECT_EQUAL(table.elems[0], { "", 0 });
    for (int i = 1; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }

    EXPECT(table.contains(""));
    EXPECT_EQUAL(table.size(), 1);

    /* Remove the empty string. */
    EXPECT(table.remove(""));
    EXPECT_EQUAL(table.size(), 0);
    EXPECT(!table.contains(""));

    /* Validate table internals. */
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }

    EXPECT(!table.remove(""));
}

PROVIDED_TEST("Doesn't backward-shift elements from their homes.") {
    /* Each element hashes to its own numeric value, modded by the table size. */
    RobinHoodHashTable table(Hash::identity(10));

    /* Insert two adjacent runs of values to get this sequence:
     *
     * .  1 11 21 31  5 15 25 35 .
     *    0  1  2  3  0  1  2  3
     */
    EXPECT(table.insert("1"));
    EXPECT(table.insert("11"));
    EXPECT(table.insert("21"));
    EXPECT(table.insert("31"));
    EXPECT(table.insert("5"));
    EXPECT(table.insert("15"));
    EXPECT(table.insert("25"));
    EXPECT(table.insert("35"));
    EXPECT_EQUAL(table.size(), 8);

    /* Validate table internals. */
    EXPECT_EQUAL(table.elems[0].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[1], {  "1", 0 });
    EXPECT_EQUAL(table.elems[2], { "11", 1 });
    EXPECT_EQUAL(table.elems[3], { "21", 2 });
    EXPECT_EQUAL(table.elems[4], { "31", 3 });
    EXPECT_EQUAL(table.elems[5], {  "5", 0 });
    EXPECT_EQUAL(table.elems[6], { "15", 1 });
    EXPECT_EQUAL(table.elems[7], { "25", 2 });
    EXPECT_EQUAL(table.elems[8], { "35", 3 });
    EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);

    /* Remove 11. This should backward-shift delete to give
     *
     * .  1 21 31  .  5 15 25 35 .
     *    0  1  2     0  1  2  3
     */
    EXPECT(table.remove("11"));
    EXPECT_EQUAL(table.size(), 7);

    EXPECT_EQUAL(table.elems[0].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[1], {  "1", 0 });
    EXPECT_EQUAL(table.elems[2], { "21", 1 });
    EXPECT_EQUAL(table.elems[3], { "31", 2 });
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5], {  "5", 0 });
    EXPECT_EQUAL(table.elems[6], { "15", 1 });
    EXPECT_EQUAL(table.elems[7], { "25", 2 });
    EXPECT_EQUAL(table.elems[8], { "35", 3 });
    EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);

    EXPECT(table.contains("1"));
    EXPECT(!table.contains("11"));
    EXPECT(table.contains("21"));
    EXPECT(table.contains("31"));
    EXPECT(table.contains("5"));
    EXPECT(table.contains("15"));
    EXPECT(table.contains("25"));
    EXPECT(table.contains("35"));
}

PROVIDED_TEST("Deletes around the end of the table.") {
    /* Everything goes in slot 8. */
    RobinHoodHashTable table(Hash::constant(10, 8));

    /* Insert five values to give back this table:
     *
     * 2  3  4  .  .  .  .  .  0  1
     * 2  3  4                 0  1
     */
    for (int i = 0; i < 5; i++) {
        EXPECT(table.insert(to_string(i)));
    }
    EXPECT_EQUAL(table.size(), 5);

    EXPECT_EQUAL(table.elems[0], {  "2", 2 });
    EXPECT_EQUAL(table.elems[1], {  "3", 3 });
    EXPECT_EQUAL(table.elems[2], {  "4", 4 });
    EXPECT_EQUAL(table.elems[3].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8], {  "0", 0 });
    EXPECT_EQUAL(table.elems[9], {  "1", 1 });

    /* Remove 0. This should shift everything back like this:
     *
     * 3  4  .  .  .  .  .  .  1  2
     * 2  3                    0  1
     */
    EXPECT(table.remove("0"));
    EXPECT_EQUAL(table.size(), 4);

    EXPECT_EQUAL(table.elems[0], {  "3", 2 });
    EXPECT_EQUAL(table.elems[1], {  "4", 3 });
    EXPECT_EQUAL(table.elems[2].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[3].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8], {  "1", 0 });
    EXPECT_EQUAL(table.elems[9], {  "2", 1 });

    /* Remove 1. This should shift everything back like this:
     *
     * 4  .  .  .  .  .  .  .  2  3
     * 2                       0  1
     */
    EXPECT(table.remove("1"));
    EXPECT_EQUAL(table.size(), 3);

    EXPECT_EQUAL(table.elems[0], {  "4", 2 });
    EXPECT_EQUAL(table.elems[1].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[2].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[3].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8], {  "2", 0 });
    EXPECT_EQUAL(table.elems[9], {  "3", 1 });

    /* Remove 2. This should shift everything back like this:
     *
     * .  .  .  .  .  .  .  .  3  4
     *                         0  1
     */
    EXPECT(table.remove("2"));
    EXPECT_EQUAL(table.size(), 2);

    EXPECT_EQUAL(table.elems[0].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[1].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[2].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[3].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8], {  "3", 0 });
    EXPECT_EQUAL(table.elems[9], {  "4", 1 });

    /* Remove 3. This should shift everything back like this:
     *
     * .  .  .  .  .  .  .  .  4  .
     *                         0
     */
    EXPECT(table.remove("3"));
    EXPECT_EQUAL(table.size(), 1);

    EXPECT_EQUAL(table.elems[0].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[1].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[2].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[3].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[4].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8], {  "4", 0 });
    EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);

    /* Remove 4. This should leave the table empty. */
    EXPECT(table.remove("4"));
    EXPECT_EQUAL(table.size(), 0);
    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }
}

PROVIDED_TEST("Removing non-existent elements has no effect.") {
    /* Drop everything in bucket 0, which is a terrible choice of hash function but
     * which makes testing a lot easier.
     */
    RobinHoodHashTable table(Hash::zero(10));

    /* Fill the table. */
    for (int i = 0; i < 5; i++) {
        EXPECT(table.insert(to_string(i)));
    }
    EXPECT_EQUAL(table.size(), 5);

    /* Validate table internals. We should see this:
     *
     * 0 1 2 3 4 . . . . .
     * 0 1 2 3 4
     */
    EXPECT_EQUAL(table.size(), 5);
    EXPECT_EQUAL(table.elems[0], {  "0", 0 });
    EXPECT_EQUAL(table.elems[1], {  "1", 1 });
    EXPECT_EQUAL(table.elems[2], {  "2", 2 });
    EXPECT_EQUAL(table.elems[3], {  "3", 3 });
    EXPECT_EQUAL(table.elems[4], {  "4", 4 });
    EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[8].distance, RobinHoodHashTable::EMPTY_SLOT);
    EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);

    /* Try removing things that don't exist. */
    for (int i = 5; i < 30; i++) {
        EXPECT(!table.remove(to_string(i)));

        /* Confirm that the table is unchanged. */
        EXPECT_EQUAL(table.size(), 5);
        EXPECT_EQUAL(table.elems[0], {  "0", 0 });
        EXPECT_EQUAL(table.elems[1], {  "1", 1 });
        EXPECT_EQUAL(table.elems[2], {  "2", 2 });
        EXPECT_EQUAL(table.elems[3], {  "3", 3 });
        EXPECT_EQUAL(table.elems[4], {  "4", 4 });
        EXPECT_EQUAL(table.elems[5].distance, RobinHoodHashTable::EMPTY_SLOT);
        EXPECT_EQUAL(table.elems[6].distance, RobinHoodHashTable::EMPTY_SLOT);
        EXPECT_EQUAL(table.elems[7].distance, RobinHoodHashTable::EMPTY_SLOT);
        EXPECT_EQUAL(table.elems[8].distance, RobinHoodHashTable::EMPTY_SLOT);
        EXPECT_EQUAL(table.elems[9].distance, RobinHoodHashTable::EMPTY_SLOT);
    }
}

PROVIDED_TEST("Can remove from a full table.") {
    /* Drop everything in bucket 7, which is a terrible choice of hash function but
     * which makes testing a lot easier.
     */
    RobinHoodHashTable table(Hash::constant(10, 7));

    /* Fill the table. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.insert(to_string(i)));
    }
    EXPECT_EQUAL(table.size(), 10);

    /* Validate table internals. We should see this:
     *
     * 3  4  5  6  7  8  9  0  1  2
     * 3  4  5  6  7  8  9  0  1  2
     */
    EXPECT_EQUAL(table.size(), 10);
    EXPECT_EQUAL(table.elems[0], {  "3", 3 });
    EXPECT_EQUAL(table.elems[1], {  "4", 4 });
    EXPECT_EQUAL(table.elems[2], {  "5", 5 });
    EXPECT_EQUAL(table.elems[3], {  "6", 6 });
    EXPECT_EQUAL(table.elems[4], {  "7", 7 });
    EXPECT_EQUAL(table.elems[5], {  "8", 8 });
    EXPECT_EQUAL(table.elems[6], {  "9", 9 });
    EXPECT_EQUAL(table.elems[7], {  "0", 0 });
    EXPECT_EQUAL(table.elems[8], {  "1", 1 });
    EXPECT_EQUAL(table.elems[9], {  "2", 2 });

    /* Try removing some elements that aren't present. This may hang if your implementation
     * of remove wasn't anticipating this case.
     */
    for (int i = 10; i < 20; i++) {
        EXPECT(!table.remove(to_string(i)));

        /* Make sure the table is unchanged. */
        EXPECT_EQUAL(table.size(), 10);

        EXPECT_EQUAL(table.elems[0], {  "3", 3 });
        EXPECT_EQUAL(table.elems[1], {  "4", 4 });
        EXPECT_EQUAL(table.elems[2], {  "5", 5 });
        EXPECT_EQUAL(table.elems[3], {  "6", 6 });
        EXPECT_EQUAL(table.elems[4], {  "7", 7 });
        EXPECT_EQUAL(table.elems[5], {  "8", 8 });
        EXPECT_EQUAL(table.elems[6], {  "9", 9 });
        EXPECT_EQUAL(table.elems[7], {  "0", 0 });
        EXPECT_EQUAL(table.elems[8], {  "1", 1 });
        EXPECT_EQUAL(table.elems[9], {  "2", 2 });
    }

    /* Now, do the actual removals. */
    for (int i = 0; i < 10; i++) {
        EXPECT(table.remove(to_string(i)));
    }

    EXPECT(table.isEmpty());
    EXPECT_EQUAL(table.size(), 0);

    for (int i = 0; i < 10; i++) {
        EXPECT_EQUAL(table.elems[i].distance, RobinHoodHashTable::EMPTY_SLOT);
    }
}

PROVIDED_TEST("Stress Test: Handles large numbers of removals (should take under a second).") {
    const int kNumTrials = 50; // Do this lots of times to smoke out any errors that might be lurking.
    for (int trial = 0; trial < kNumTrials; trial++) {
        RobinHoodHashTable table(Hash::random(100));

        const int kNumElems = 75;
        for (int i = 0; i < kNumElems; i++) {
            table.insert(to_string(i));
        }

        EXPECT_EQUAL(table.size(), kNumElems);

        for (int i = 0; i < kNumElems; i++) {
            /* Confirm only the proper elements exist at this point. */
            for (int j = 0; j < kNumElems; j++) {
                EXPECT_EQUAL(table.contains(to_string(j)), j >= i);
            }

            EXPECT(table.remove(to_string(i)));
            EXPECT_EQUAL(table.size(), kNumElems - i - 1);
        }
    }
}

PROVIDED_TEST("Stress Test: Inserts/searches/deletes work in expected time O(1).") {
    /* Huge number of slots. */
    const int kNumSlots = 1000000;

    /* Create an enormous hash table with a random hash function. */
    RobinHoodHashTable table(Hash::random(kNumSlots));

    /* Insert a lot elements. */
    const int kLotsOfElems = 100000; // 10% load factor - quite small!
    for (int i = 0; i < kLotsOfElems; i++) {
        EXPECT(table.insert(to_string(i)));
    }

    /* Remove the middle half of them. */
    for (int i = kLotsOfElems / 4; i < 3 * kLotsOfElems / 4; i++) {
        EXPECT(table.remove(to_string(i)));
    }

    /* Search for lots of elements and confirm the ones are supposed to be there
     * are indeed there.
     */
    for (int i = 0; i < kLotsOfElems; i++) {
        EXPECT_EQUAL(table.contains(to_string(i)), bool(i < kLotsOfElems / 4 || i >= 3 * kLotsOfElems / 4));
    }
}

#include "filelib.h"
#include "Demos/Timer.h"
#include <fstream>
PROVIDED_TEST("Stress Test: Handles large workflows with little free space (should take at most five seconds)") {
    // SHOW_ERROR("Stress test is disabled by default. To run it, comment out line " + to_string(__LINE__) + " of " + getTail(__FILE__) + ".");

    Vector<string> english;
    ifstream input("res/EnglishWords.txt");

    for (string word; getline(input, word); ) {
        english += word;
    }

    /* Load factor 0.99. */
    Timing::Timer timer;
    timer.start();
    RobinHoodHashTable table(Hash::consistentRandom(english.size() / 0.99));

    /* Insert everything. */
    for (const string& word: english) {
        EXPECT(table.insert(word));
    }
    EXPECT_EQUAL(table.size(), english.size());

    /* Make sure everything is there, and that the upper-case versions aren't. */
    for (const string& word: english) {
        EXPECT(table.contains(word));
        EXPECT(!table.contains(toUpperCase(word)));
    }

    /* Remove everything, plus some things not there.. */
    for (const string& word: english) {
        EXPECT(table.remove(word));
        EXPECT(!table.contains(word));
        EXPECT(!table.remove(toUpperCase(word)));
    }

    EXPECT_EQUAL(table.size(), 0);
    EXPECT(table.isEmpty());

    timer.stop();
    EXPECT_LESS_THAN(timer.elapsed(), 5e9); // Measured in nanoseconds
}
