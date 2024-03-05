// Author: Sean Szumlanski
// Date: Autumn 2023
//
// Sorting algorithms implementations:
//
// - Selection Sort
// - Insertion Sort
// - Merge Sort


#include <iostream>
#include "console.h"
#include "random.h"
#include "vector.h"
#include "SimpleTest.h"
using namespace std;


// Creates and returns a vector of n random integers on the range 1 through 1,000,000.
Vector<int> createRandoVector(int n)
{
    Vector<int> v;

    for (int i = 0; i < n; i++)
    {
        v.add(randomInteger(1, 1000000));
    }

    return v;
}

// Swaps values at indices i and j in given vector. Assumes indices are valid.
void swap(Vector<int>& v, int i, int j)
{
    int temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

// Returns true if given vector is sorted, false otherwise.
bool isSorted(Vector<int>& v)
{
    for (int i = 0; i < v.size() - 1; i++)
    {
        if (v[i] > v[i + 1])
        {
            return false;
        }
    }

    return true;
}

void selectionSort(Vector<int>& v)
{
    for (int start = 0; start < v.size(); start++)
    {
        int indexOfMin = start;

        for (int j = start + 1; j < v.size(); j++)
        {
            if (v[j] < v[indexOfMin])
            {
                indexOfMin = j;
            }
        }

        swap(v, start, indexOfMin);
    }
}

void insertionSort(Vector<int>& v)
{
    for (int start = 1; start < v.size(); start++)
    {
        int peach = v[start];
        int gap;

        for (gap = start; gap > 0 && peach < v[gap - 1]; gap--)
        {
            v[gap] = v[gap - 1];
        }

        v[gap] = peach;
    }
}

void mergeSort(Vector<int>& v, int lo, int hi)
{
    // Base case: one (or fewer) elements in this portion of the vector.
    if (lo >= hi)
    {
        return;
    }

    int mid = lo + (hi - lo) / 2;

    // Sort left and right halves recursively.
    mergeSort(v, lo, mid);
    mergeSort(v, mid + 1, hi);

    // Merge results back together in auxiliary vector.
    Vector<int> aux;

    int i = lo;
    int j = mid + 1;

    while (i <= mid || j <= hi)
    {
        if (i > mid)
        {
            // If i is invalid, j must be valid.
            aux.add(v[j]);
            j++;
        }
        else if (j > hi)
        {
            // If j is invalid, i must be valid.
            aux.add(v[i]);
            i++;
        }
        else if (v[i] < v[j])
        {
            // If both i and j are valid, we check whether
            // v[i] < v[j] or not. We want the smaller value.
            aux.add(v[i]);
            i++;
        }
        else
        {
            // This is the case where i and j are valid, and
            // v[j] <= v[i].
            aux.add(v[j]);
            j++;
        }
    }

    // Copy everything from the auxiliary vector back into the original.
    for (i = lo; i <= hi; i++)
    {
        v[i] = aux[i - lo];
    }
}

// Wrapper function. Sort given vector using recursive mergesort.
void mergeSort(Vector<int>& v)
{
    mergeSort(v, 0, v.size() - 1);
}

PROVIDED_TEST("merge sort runtime")
{
    int n = 1000;
    int numRuns = 5;

    for (int i = 0; i < numRuns; i++)
    {
        Vector<int> v = createRandoVector(n);
        TIME_OPERATION(v.size(), mergeSort(v));
        EXPECT(isSorted(v));
    }
}

PROVIDED_TEST("selection sort runtime")
{
    int n = 1000;
    int numRuns = 5;

    for (int i = 0; i < numRuns; i++)
    {
        Vector<int> v = createRandoVector(n);
        TIME_OPERATION(v.size(), selectionSort(v));
        EXPECT(isSorted(v));
    }
}

PROVIDED_TEST("insertion sort runtime")
{
    int n = 1000;
    int numRuns = 5;

    for (int i = 0; i < numRuns; i++)
    {
        Vector<int> v = createRandoVector(n);
        TIME_OPERATION(v.size(), insertionSort(v));
        EXPECT(isSorted(v));
    }
}

int main()
{
    runSimpleTests(ALL_TESTS);
    return 0;
}
