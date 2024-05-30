#include "DisasterPlanning.h"

using namespace std;

/* TODO: Refer to DisasterPlanning.h for more information about this function.
 * Then, delete this comment.
 */
bool helper(const Map<string, Set<string>> & roadNetwork,
            int numCitys,
            Set<string>& supplyLocations,
            Set<string>& remaining) {
    if (numCitys < 0) {
        return false;
    }
    if (remaining.isEmpty() && numCitys >= 0) {
        return true;
    }
    // case 1:
    // choose one of the remaining to supplyLocations
    string choose = remaining.first();
    Set<string> neighbors = roadNetwork[choose];
    Set<string> left = remaining - choose - neighbors;
    supplyLocations += choose;     // choose
    if (helper(roadNetwork, numCitys - 1, supplyLocations, left)) {  // explore
        return true;
    }
    supplyLocations -= choose;  // unchoose

    // case 2: choose one of the remaining's neighbor to supplyLocations
    for (string neighbor : neighbors) {
        Set<string> neighborsOfNeighbor = roadNetwork[neighbor];
        Set<string> left = remaining - neighborsOfNeighbor - neighbor;
        supplyLocations += neighbor;
        if (helper(roadNetwork, numCitys - 1, supplyLocations, left)) {
            return true;
        }
        supplyLocations -= neighbor;
    }
    return false;
}

bool canBeMadeDisasterReady(const Map<string, Set<string>>& roadNetwork,
                            int numCities,
                            Set<string>& supplyLocations) {
    if (numCities < 0) {
        error("num of supplied city less than 0");
    }
    Set<string> remaining;
    for (string city : roadNetwork) {
        remaining += city;
    }
    return helper(roadNetwork, numCities, supplyLocations, remaining);
}


/* * * * * * * Test Helper Functions Below This Point * * * * * */
#include "GUI/SimpleTest.h"

/* This is a helper function that's useful for designing test cases. You give it a Map
 * of cities and what they're adjacent to, and it then produces a new Map where if city
 * A links to city B, then city B links back to city A. We recommend using this helper
 * function when writing tests, though you won't need it in your implementation of the main
 * canBeMadeDisasterReady function.
 */
Map<string, Set<string>> makeSymmetric(const Map<string, Set<string>>& source) {
    Map<string, Set<string>> result = source;

    for (const string& from: source) {
        for (const string& to: source[from]) {
            result[from] += to;
            result[to] += from;
        }
    }

    return result;
}

/* This helper function tests whether a city has been covered by a set of supply locations
 * and is used by our testing code. You're welcome to use it in your tests as well!
 */
bool isCovered(const string& city,
               const Map<string, Set<string>>& roadNetwork,
               const Set<string>& supplyLocations) {
    if (supplyLocations.contains(city)) return true;

    for (string neighbor: roadNetwork[city]) {
        if (supplyLocations.contains(neighbor)) return true;
    }

    return false;
}

/* * * * * * Test Cases Below This Point * * * * * */

/* TODO: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Reports an error if numCities < 0") {
    Set<string> supply;
    EXPECT_ERROR(canBeMadeDisasterReady({}, -137, supply));
}

PROVIDED_TEST("Works for map with no cities.") {
    Set<string> locations;

    /* The number of cities we use really doesn't matter here. */
    EXPECT(canBeMadeDisasterReady({}, 0, locations));
    EXPECT_EQUAL(locations.size(), 0);

    EXPECT(canBeMadeDisasterReady({}, 137, locations));
    EXPECT_EQUAL(locations.size(), 0);
}

PROVIDED_TEST("Works for map with one city.") {
    Map<string, Set<string>> map = makeSymmetric({
         { "Solipsist", {} }
    });

    /* Shouldn't matter how many cities we use, as long as it isn't zero! */
    Set<string> locations0, locations1, locations2;
    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
    EXPECT( canBeMadeDisasterReady(map, 1, locations1));
    EXPECT( canBeMadeDisasterReady(map, 2, locations2));
}

PROVIDED_TEST("Works for map with one city, and produces output.") {
    Map<string, Set<string>> map = makeSymmetric({
         { "Solipsist", {} }
    });

    Set<string> locations0, locations1, locations2;
    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
    EXPECT(canBeMadeDisasterReady(map, 1, locations1));
    EXPECT(canBeMadeDisasterReady(map, 2, locations2));

    /* Don't check locations0; since the function returned false, the values there
     * can be anything.
     */
    Set<string> expected = { "Solipsist" };
    EXPECT_EQUAL(locations1, expected);
    EXPECT_EQUAL(locations2, expected);
}

PROVIDED_TEST("Works for map with two linked cities.") {
    Map<string, Set<string>> map = makeSymmetric({
         { "A", { "B" } },
         { "B", {     } }
    });

    Set<string> locations0, locations1, locations2;
    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
    EXPECT(canBeMadeDisasterReady(map, 1, locations1));
    EXPECT(canBeMadeDisasterReady(map, 2, locations2));
}

PROVIDED_TEST("Works for map with two linked cities, and produces output.") {
    Map<string, Set<string>> map = makeSymmetric({
         { "A", { "B" } },
    });

    Set<string> locations0, locations1, locations2;
    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
    EXPECT(canBeMadeDisasterReady(map, 1, locations1));
    EXPECT(canBeMadeDisasterReady(map, 2, locations2));

    EXPECT_EQUAL(locations1.size(), 1);
    EXPECT(locations1.isSubsetOf({"A", "B"}));

    EXPECT(locations2.size() <= 2);
    EXPECT(locations2.isSubsetOf({"A", "B"}));
}

PROVIDED_TEST("Works for four disconnected cities.") {
    Map<string, Set<string>> map = makeSymmetric({
        { "A", { } },
        { "B", { } },
        { "C", { } },
        { "D", { } }
    });

    Set<string> locations0, locations1, locations2, locations3, locations4;
    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
    EXPECT(!canBeMadeDisasterReady(map, 1, locations1));
    EXPECT(!canBeMadeDisasterReady(map, 2, locations2));
    EXPECT(!canBeMadeDisasterReady(map, 3, locations3));
    EXPECT(canBeMadeDisasterReady(map, 4, locations4));
}

PROVIDED_TEST("Works for four disconnected cities, and produces output.") {
    Map<string, Set<string>> map = makeSymmetric({
        { "A", { } },
        { "B", { } },
        { "C", { } },
        { "D", { } }
    });

    Set<string> locations0, locations1, locations2, locations3, locations4;
    EXPECT(!canBeMadeDisasterReady(map, 0, locations0));
    EXPECT(!canBeMadeDisasterReady(map, 1, locations1));
    EXPECT(!canBeMadeDisasterReady(map, 2, locations2));
    EXPECT(!canBeMadeDisasterReady(map, 3, locations3));
    EXPECT(canBeMadeDisasterReady(map, 4, locations4));

    Set<string> expected = { "A", "B", "C", "D" };
    EXPECT_EQUAL(locations4, expected);
}

PROVIDED_TEST("Can solve ethene example, regardless of ordering.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *             *
     *             |
     *        * -- * -- * -- *
     *                  |
     *                  *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities. If your code is able to solve the problem correctly for all of
     * those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Ethene arrangement:
         *
         *        0
         *       1234
         *         5
         */
        Map<string, Set<string>> map = makeSymmetric({
            { cities[2], { cities[0], cities[1], cities[3] } },
            { cities[3], { cities[4], cities[5] } }
        });

        /* We should be able to cover everything with two cities:
         * city 2 and city 3.
         */
        Set<string> chosen;
        EXPECT(canBeMadeDisasterReady(map, 2, chosen));

        /* We should not be able to cover everything with one city. */
        chosen.clear();
        EXPECT(!canBeMadeDisasterReady(map, 1, chosen));
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Can solve ethene example, regardless of ordering, and produces output.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *             *
     *             |
     *        * -- * -- * -- *
     *                  |
     *                  *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities. If your code is able to solve the problem correctly for all of
     * those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Ethene arrangement:
         *
         *        0
         *       1234
         *         5
         */
        Map<string, Set<string>> map = makeSymmetric({
            { cities[2], { cities[0], cities[1], cities[3] } },
            { cities[3], { cities[4], cities[5] } }
        });

        /* We should be able to cover everything with two cities:
         * city 2 and city 3.
         */
        Set<string> chosen;
        EXPECT(canBeMadeDisasterReady(map, 2, chosen));

        EXPECT_EQUAL(chosen.size(), 2);
        EXPECT(chosen.contains(cities[2]));
        EXPECT(chosen.contains(cities[3]));

        /* We should not be able to cover everything with one city. */
        chosen.clear();
        EXPECT(!canBeMadeDisasterReady(map, 1, chosen));
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Works for six cities in a line, regardless of order.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *        * -- * -- * -- * -- * -- *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities in a line. If your code is able to solve the problem correctly
     * for all of those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Linear arrangement. */
        Map<string, Set<string>> map;
        for (int i = 1; i + 1 < cities.size(); i++) {
            map[cities[i]] = { cities[i - 1], cities[i + 1] };
        }

        map = makeSymmetric(map);

        /* We should be able to cover everything with two cities, specifically,
         * the cities one spot in from the two sides.
         */
        Set<string> chosen;
        EXPECT(canBeMadeDisasterReady(map, 2, chosen));

        /* We should not be able to cover everything with one city. */
        chosen.clear();
        EXPECT(!canBeMadeDisasterReady(map, 1, chosen));
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Works for six cities in a line, regardless of order, and produces output.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *        * -- * -- * -- * -- * -- *
     *
     * There are 6! = 720 possible permutations of the ordering of these six
     * cities in a line. If your code is able to solve the problem correctly
     * for all of those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F" };
    do {
        /* Linear arrangement. */
        Map<string, Set<string>> map;
        for (int i = 1; i + 1 < cities.size(); i++) {
            map[cities[i]] = { cities[i - 1], cities[i + 1] };
        }

        map = makeSymmetric(map);

        /* We should be able to cover everything with two cities, specifically,
         * the cities one spot in from the two sides.
         */
        Set<string> chosen;
        EXPECT(canBeMadeDisasterReady(map, 2, chosen));

        EXPECT_EQUAL(chosen.size(), 2);
        EXPECT(chosen.contains(cities[1]));
        EXPECT(chosen.contains(cities[4]));

        /* We should not be able to cover everything with one city. */
        chosen.clear();
        EXPECT(!canBeMadeDisasterReady(map, 1, chosen));
    } while (next_permutation(cities.begin(), cities.end()));
}

/* The "Don't Be Greedy" sample world. */
const Map<string, Set<string>> kDontBeGreedy = makeSymmetric({
    { "A", { "B" } },
    { "B", { "C", "D" } },
    { "C", { "D" } },
    { "D", { "F", "G" } },
    { "E", { "F" } },
    { "F", { "G" } },
});

PROVIDED_TEST("Solves \"Don't be Greedy\" from the handout.") {
    Set<string> locations0, locations1, locations2;
    EXPECT(!canBeMadeDisasterReady(kDontBeGreedy, 0, locations0));
    EXPECT(!canBeMadeDisasterReady(kDontBeGreedy, 1, locations1));
    EXPECT( canBeMadeDisasterReady(kDontBeGreedy, 2, locations2));
}

PROVIDED_TEST("Solves \"Don't be Greedy\" from the handout, and produces output.") {
    Set<string> locations0, locations1, locations2;
    EXPECT(!canBeMadeDisasterReady(kDontBeGreedy, 0, locations0));
    EXPECT(!canBeMadeDisasterReady(kDontBeGreedy, 1, locations1));
    EXPECT( canBeMadeDisasterReady(kDontBeGreedy, 2, locations2));

    Set<string> expected = {"B", "F"};
    EXPECT_EQUAL(locations2, expected);
}

PROVIDED_TEST("Solves \"Don't be Greedy,\" regardless of ordering, and produces output.") {
    /* Because Map and Set internally store items in sorted order, the order
     * in which you iterate over the cities when making decisions is sensitive
     * to the order of those cities' names. This test looks at a map like
     * this one, trying out all possible orderings of the city names:
     *
     *     0       4
     *     |       |
     *     1 - 2 - 3
     *      \ / \ /
     *       5   6
     *
     * There are 7! = 5,040 possible permutations of the ordering of these seven
     * cities. If your code is able to solve the problem correctly for all of
     * those orderings, there's a good chance that you're correctly
     * covering and uncovering cities at each step. On the other hand, if
     * your code runs into issues here, it may indicate that the way in which
     * you're covering and uncovering cities accidentally uncovers a city that
     * you have previously covered.
     */
    Vector<string> cities = { "A", "B", "C", "D", "E", "F", "G" };
    do {
        Map<string, Set<string>> map = makeSymmetric({
            { cities[1], { cities[0], cities[2], cities[5] } },
            { cities[2], { cities[3], cities[5], cities[6] } },
            { cities[3], { cities[4], cities[6] } },
        });

        /* We should be able to cover everything with two cities. */
        Set<string> chosen;
        EXPECT(canBeMadeDisasterReady(map, 2, chosen));

        /* Those cities should be 1 and 3. */
        EXPECT_EQUAL(chosen, { cities[1], cities[3] });

        /* We should not be able to cover everything with one city. */
        chosen.clear();
        EXPECT(!canBeMadeDisasterReady(map, 1, chosen));
    } while (next_permutation(cities.begin(), cities.end()));
}

PROVIDED_TEST("Stress test: 6 x 6 grid. (This should take at most a few seconds.)") {
    Map<string, Set<string>> grid;

    /* Build the grid. */
    char maxRow = 'F';
    int  maxCol = 6;
    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            if (row != maxRow) {
                grid[row + to_string(col)] += (char(row + 1) + to_string(col));
            }
            if (col != maxCol) {
                grid[row + to_string(col)] += (char(row) + to_string(col + 1));
            }
        }
    }
    grid = makeSymmetric(grid);

    Set<string> locations;
    EXPECT(canBeMadeDisasterReady(grid, 10, locations));
}

PROVIDED_TEST("Stress test: 6 x 6 grid, with output. (This should take at most a few seconds.)") {
    Map<string, Set<string>> grid;

    /* Build the grid. */
    char maxRow = 'F';
    int  maxCol = 6;
    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            if (row != maxRow) {
                grid[row + to_string(col)] += (char(row + 1) + to_string(col));
            }
            if (col != maxCol) {
                grid[row + to_string(col)] += (char(row) + to_string(col + 1));
            }
        }
    }
    grid = makeSymmetric(grid);

    Set<string> locations;
    EXPECT(canBeMadeDisasterReady(grid, 10, locations));

    for (char row = 'A'; row <= maxRow; row++) {
        for (int col = 1; col <= maxCol; col++) {
            EXPECT(isCovered(row + to_string(col), grid, locations));
        }
    }
}

