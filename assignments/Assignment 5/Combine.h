#ifndef Combine_Included
#define Combine_Included

#include "Demos/DataPoint.h"
#include "vector.h"

/**
 * Given a list of several different sorted sequences, combines all those sorted sequences
 * together into an overall, large sorted sequence.
 *
 * Each sequence is represented as a Vector<DataPoint>, where the DataPoints are sorted
 * by weight. The sequences don't necessarily have to have the same length, and in fact
 * sequences may be entirely empty.
 *
 * This function should run in time O(n log k), where n is the total number of elements
 * across all those sequences and k is the number of sequences.
 *
 * There may be many data points in the sequences that have the same weight. If so, feel
 * free to break ties however you'd like.
 *
 * @param toMerge The list of sorted sequences to combine. Each sequence is sorted, by weight,
 *                in ascending order.
 * @return A list of all of the elements of those sequences in sorted order.
 */
Vector<DataPoint> combine(const Vector<Vector<DataPoint>>& toMerge);

#endif
