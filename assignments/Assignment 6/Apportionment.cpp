#include "Apportionment.h"
#include "HeapPQueue.h"
#include <cmath>
using namespace std;

Map<string, int> apportion(const Map<string, int>& populations, int numSeats) {
    Map<string, int> result;
    for (string state : populations) {
        result[state] = 1;
        numSeats--;
    }
    if (numSeats < 0) error("the number of seats less then states");
    HeapPQueue PQ;
    for (string state : populations) {
        PQ.enqueue({state, -populations[state]/sqrt(2)});
    }
    while (numSeats >= 1) {
        DataPoint choose = PQ.dequeue();
        result[choose.name]++;
        numSeats--;
        int s = result[choose.name];
        choose.weight = (-populations[choose.name]) / sqrt(s * (s+1));
        PQ.enqueue({choose});
    }
    return result;
}


/* * * * * Test Cases Below This Point * * * * */
#include "GUI/SimpleTest.h"
#include "Utilities/CSV.h"

PROVIDED_TEST("Each state gets at least one seat.") {
    /* Fall 2020 */
    Map<string, int> populations = {
        { "CS106A", 551 },
        { "CS106B", 428 },
        { "CS103",  206 },
        { "CS107",  267 },
        { "CS109",  214 },
        { "CS110",  196 },
        { "CS161",  187 }
    };

    /* Seven seats and seven classes means everyone gets one. */
    Map<string, int> seats = {
        { "CS106A", 1 },
        { "CS106B", 1 },
        { "CS103",  1 },
        { "CS107",  1 },
        { "CS109",  1 },
        { "CS110",  1 },
        { "CS161",  1 }
    };

    EXPECT_EQUAL(apportion(populations, 7), seats);
}

PROVIDED_TEST("Reports an error if there are more states than seats.") {
    Map<string, int> populations = {
        { "House",         435 },
        { "Senate",        100 },
        { "Supreme Court",   9 },
        { "President",       1 },
    };

    EXPECT_ERROR(apportion(populations, 3));
    EXPECT_ERROR(apportion(populations, -2));
    EXPECT_NO_ERROR(apportion(populations, 4));
}

PROVIDED_TEST("Works on the example from the handout.") {
    Map<string, int> populations = {
        { "Mind",    25419 },
        { "Denial",  13509 },
        { "Affairs", 7984  },
        { "Being",   14938 },
        { "Matter",  14617 }
    };

    Map<string, int> seats = {
        { "Mind",    5 },
        { "Denial",  2 },
        { "Affairs", 1 },
        { "Being",   3 },
        { "Matter",  3 }
    };

    EXPECT_EQUAL(apportion(populations, 14), seats);
}

namespace {
    Map<string, int> populationsFrom(const string& filename) {
        CSV csv = CSV::parseFile(filename);

        Map<string, int> populations;
        for (size_t i = 0; i < csv.numRows(); i++) {
            populations[csv[i]["State"]] = stringToInteger(csv[i]["Population"]);
        }

        return populations;
    }
}

PROVIDED_TEST("Works on the 2020 apportionment.") {
    auto populations = populationsFrom("res/apportionment/2020.csv");

    /* Apportioned results. */
    Map<string, int> seats = {
        { "Alabama", 7 },
        { "Alaska", 1 },
        { "Arizona", 9 },
        { "Arkansas", 4 },
        { "California", 52 },
        { "Colorado", 8 },
        { "Connecticut", 5 },
        { "Delaware", 1 },
        { "Florida", 28 },
        { "Georgia", 14 },
        { "Hawaii", 2 },
        { "Idaho", 2 },
        { "Illinois", 17 },
        { "Indiana", 9 },
        { "Iowa", 4 },
        { "Kansas", 4 },
        { "Kentucky", 6 },
        { "Louisiana", 6 },
        { "Maine", 2 },
        { "Maryland", 8 },
        { "Massachusetts", 9 },
        { "Michigan", 13 },
        { "Minnesota", 8 },
        { "Mississippi", 4 },
        { "Missouri", 8 },
        { "Montana", 2 },
        { "Nebraska", 3 },
        { "Nevada", 4 },
        { "New Hampshire", 2 },
        { "New Jersey", 12 },
        { "New Mexico", 3 },
        { "New York", 26 },
        { "North Carolina", 14 },
        { "North Dakota", 1 },
        { "Ohio", 15 },
        { "Oklahoma", 5 },
        { "Oregon", 6 },
        { "Pennsylvania", 17 },
        { "Rhode Island", 2 },
        { "South Carolina", 7 },
        { "South Dakota", 1 },
        { "Tennessee", 9 },
        { "Texas", 38 },
        { "Utah", 4 },
        { "Vermont", 1 },
        { "Virginia", 11 },
        { "Washington", 10 },
        { "West Virginia", 2 },
        { "Wisconsin", 8 },
        { "Wyoming", 1 },
    };

    EXPECT_EQUAL(apportion(populations, 435), seats);
}

PROVIDED_TEST("Stress test: works on a large number of states (should take at most a few seconds).") {
    const int kLargeNumber = 100000;

    /* kLargeNumber of states with population 1. */
    Map<string, int> populations;
    for (int i = 0; i < kLargeNumber; i++) {
        populations[to_string(i)] = 1;
    }

    /* One state with population kLargeNumber. */
    populations[to_string(kLargeNumber)] = kLargeNumber;

    /* Apportion kLargeNumber + (kLargeNumber / 4 - 1) seats. This has the effect
     * of giving everyone one seat, then giving kLargeNumber the remaining
     * kLargeNumber / 4 - 1 seats.
     */
    const int kExtraSeats = kLargeNumber / 4 - 1;

    auto result = apportion(populations, kLargeNumber + kExtraSeats);

    /* Confirm all the small states get one seat. */
    for (int i = 0; i < kLargeNumber; i++) {
        EXPECT_EQUAL(result[to_string(i)], 1);
    }

    /* Confirm the big state gets the rest. */
    EXPECT_EQUAL(result[to_string(kLargeNumber)], kExtraSeats);
}
