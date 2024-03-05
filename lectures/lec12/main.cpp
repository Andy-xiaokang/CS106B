#include <iostream>
#include "console.h"
#include "vector.h"
#include "SimpleTest.h"
using namespace std;

struct treasureT
{
    int weight;
    int value;
};

// PLACE ONE OF THE KNAPSACK SOLUTION FUNCTIONS HERE

// Assumes weights and values vectors have equal size.
Vector<treasureT> createTreasureVector(Vector<int>& weights, Vector<int>& values)
{
    Vector<treasureT> treasures;

    for (int i = 0; i < weights.size(); i++)
    {
        treasureT myTreasure;
        myTreasure.weight = weights[i];
        myTreasure.value = values[i];
        treasures.add(myTreasure);
    }

    return treasures;
}

// Returns the maximum value we can derive by taking treasures from the given vector
// (without the total weight of the treasures we take exceeding the given capacity).
int knapsack(Vector<treasureT> treasures, int capacity)
{
    // No more treasures to choose from means no more value to gain. Finito!
    if (treasures.size() == 0)
    {
        return 0;
    }

    // Here, we pull an item out of our vector in preparation for making a binary
    // choice about what to do with it: either take it or leave it behind.
    //
    // Removing from (and later adding to) the last position in our vector is more
    // efficient than operating on index 0 (zero). The latter requires us to scooch
    // over all the rest of the elements in the vector.
    treasureT thisOne = treasures[treasures.size() - 1];
    treasures.remove(treasures.size() - 1);

    int result;

    if (thisOne.weight <= capacity)
    {
        // Take it. Adding the current item to our knapsack adds to our running value
        // but also reduces the capacity of our knapsack.
        int with = thisOne.value + knapsack(treasures, capacity - thisOne.weight);

        // Don't take it. Leaving this item behind in hopes of finding other, better
        // items with which to fill our knapsack means we don't gain any value, but
        // we also don't reduce the capacity of our knapsack.
        int without = knapsack(treasures, capacity);

        result = max(with, without);
    }
    else
    {
        // Don't take it. This is the branch where thisOne is too heavy to take. So,
        // we don't have the luxury of making a binary choice about whether to take
        // the item with us. We must leave it behind.
        result = knapsack(treasures, capacity);
    }

    // Unchoose. See notes above this function about why we must add this back.
    // treasures.add(thisOne);
    return result;
}

PROVIDED_TEST("simple knapsack test")
{
    Vector<int> weights = {4, 2, 3, 1, 6, 4};
    Vector<int> values = {6, 4, 5, 3, 9, 7};

    Vector<treasureT> treasures = createTreasureVector(weights, values);
    int capacity = 10;

    EXPECT_EQUAL(knapsack(treasures, capacity), 19);
}

PROVIDED_TEST("another simple knapsack test")
{
    Vector<int> weights = {8, 3, 3, 3, 1};
    Vector<int> values = {160, 58, 58, 58, 2};

    Vector<treasureT> treasures = createTreasureVector(weights, values);
    int capacity = 10;

    EXPECT_EQUAL(knapsack(treasures, capacity), 176);
}

int main()
{
    runSimpleTests(ALL_TESTS);
    return 0;
}
