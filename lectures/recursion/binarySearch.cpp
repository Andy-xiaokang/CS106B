#include <iostream>
#include "vector.h"
#include "console.h"
#include "random.h"
#include "simpio.h"

using namespace std;

int binarySearch(Vector<int>& v, int key, int lo, int hi) {
    if (lo > hi) {
        return -1;
    }
    int mid = lo + (hi - lo) / 2;
    if (key < v[mid]) {
        return binarySearch(v, key, lo, mid - 1);
    } else if (key > v[mid]) {
        return binarySearch(v, key, mid + 1, hi);
    } else {
        return mid;
    }
}

int binarySearch(Vector<int>& v, int key) {
    return binarySearch(v, key, 0, v.size() - 1);
}

Vector<int> createRandomSortedVector(int n) {
    Vector<int> v;
    for (int i = 0; i < n; i++) {
        v.add(randomInteger(0, 100));
    }
    v.sort();
    return v;
}

int main_binarySearch() {
    Vector<int> sortedVector = createRandomSortedVector(50);
    cout << sortedVector << endl;
    int key;
    while (true) {
        key = getInteger("please enter a interger between 0-100 (-1 to quit)");
        if (key == -1) {
            break;
        }
        cout << "result is " << binarySearch(sortedVector, key) << endl;
    }
    return 0;
}
