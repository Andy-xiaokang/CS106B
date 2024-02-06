#include <iostream>
#include "console.h"
#include "vector.h"
#include "SimpleTest.h"

using namespace std;

bool isPartitionable(Vector<int>& v, int sum1, int sum2) {
    if (v.isEmpty()) {
        return sum1 == sum2;
    }
    // choose
    int choose = v.remove(0);
    // explore
    bool result = isPartitionable(v, sum1 + choose, sum2) || isPartitionable(v, sum1, sum2 + choose);
    //unchoose
    v.insert(0, choose);
    return result;
}

bool isPartitionable(Vector<int>& v) {
    return isPartitionable(v, 0, 0);
}

STUDENT_TEST("test isPartionable") {
    Vector<int> v1 = {1, 2, 3, 4};
    Vector<int> v2 = {2, 3, 5, 6,9};
    EXPECT_EQUAL(isPartitionable(v1), true);
    EXPECT_EQUAL(isPartitionable(v2), false);
}

int main_isPartitionable() {
    if (runSimpleTests(ALL_TESTS)) {
        return 0;
    }
    return -1;
    // Vector<int> v;

    // v = {1, 1, 2, 3, 5};
    // cout << isPartitionable(v) << endl;  // true

    // v = {1, 4, 5, 6};
    // cout << isPartitionable(v) << endl;  // false

    // return 0;
}

