#include <iostream>
#include "console.h"
#include "vector.h"
#include "SimpleTest.h"
#include "random.h"
using namespace std;

bool isSorted(Vector<int> v) {
    for (int i = 0; i < v.size() - 1; i++) {
        if (v[i] > v[i+1]) {
            return false;
        }
    }
    return true;
}

// creare random number form 1 to 1,000,000 with size n
Vector<int> createRandomVector(int n) {
    Vector<int> v;
    for (int i = 0; i < n; i++) {
        v.add(randomInteger(1, 1000000));
    }
    return v;
}

void swap(Vector<int>& v, int i, int j) {
    int temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

// selectionSort
void selectionSort(Vector<int>& v) {
    for (int i = 0; i < v.size() - 1; i++) {
        int indexOfMin = i;
        for (int j = i + 1; j < v.size(); j++) {
            if (v[j] < v[indexOfMin]) {
                indexOfMin = j;
            }
        }
        swap(v, i, indexOfMin);
    }
}

void insertionSort(Vector<int>& v) {
    for (int start = 1; start < v.size(); start++) {
        int card = v[start];
        int index;
        for (index = start; index > 0 && card < v[index-1]; index--) {
            v[index] = v[index-1];
        }
        v[index] = card;
    }
}

void merge(Vector<int>& v, Vector<int>& aux, int low, int mid, int high) {
    for (int k = low; k <= high; k++) {
        aux[k] = v[k];
    }
    int i = low, j = mid + 1;
    for (int k = low; k <= high; k++) {
        if (i > mid) {
            v[k] = aux[j++];
        } else if (j > high) {
            v[k] = aux[i++];
        } else if (aux[i] < aux[j]) {
            v[k] = aux[i++];
        } else {
            v[k] = aux[j++];
        }
    }
}

void mergeSort(Vector<int>& v, Vector<int>& aux, int low, int high) {
    if (high <= low) {
        return;
    }
    int mid = low + (high - low) / 2;
    mergeSort(v, aux, low, mid);
    mergeSort(v, aux, mid + 1, high);
    merge(v, aux, low, mid, high);
}

void mergeSort(Vector<int>& v) {
    Vector<int> aux;
    for (int i = 0; i < v.size(); i++) {
        aux.add(v[i]);
    }
    mergeSort(v, aux, 0, v.size() - 1);
}

STUDENT_TEST("test selection sort") {
    int size = 1000;
    for (int i = size; i <= size * 16; i = i * 2) {
        Vector<int> v = createRandomVector(i);
        TIME_OPERATION(v.size(), selectionSort(v));
        EXPECT(isSorted(v));
    }
}

STUDENT_TEST("test insertion sort") {
    int size = 1000;
    for (int i = size; i <= size * 16; i = i * 2) {
        Vector<int> v = createRandomVector(i);
        TIME_OPERATION(v.size(), insertionSort(v));
        EXPECT(isSorted(v));
    }
}

STUDENT_TEST("test merge sort") {
    int size = 1000;
    for (int i = size; i <= size * 16; i = i * 2) {
        Vector<int> v = createRandomVector(i);
        TIME_OPERATION(v.size(), mergeSort(v));
        EXPECT(isSorted(v));
    }
}

int main() {
    runSimpleTests(SELECTED_TESTS);
    return 0;
}
