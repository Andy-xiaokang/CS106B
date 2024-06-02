#include "BigOFunctions.h"
#include <iostream>
using namespace std;

/******************************************************************************
 ******************************************************************************
 *********                 Part One: Printing Chip                    *********
 ******************************************************************************
 ******************************************************************************/

/* Prints a letter H that's n characters wide and n characters
 * tall. For example, calling printH(5) prints
 *
 *       *   *
 *       *   *
 *       *****
 *       *   *
 *       *   *
 *
 * and calling printH(7) prints
 *
 *       *     *
 *       *     *
 *       *     *
 *       *******
 *       *     *
 *       *     *
 *       *     *
 */
void printH(int n) {
    for (int row = 0; row < n; row++) {
        for (int col = 0; col < n; col++) {
            /* The character is a star if it's in the very first or very last
             * column, or if it's in the middle row.
             */
            if (row == n / 2 || col == 0 || col == n - 1) {
                cout << '*';
            }
            /* Otherwise, it's blank. */
            else {
                cout << ' ';
            }
        }
        cout << endl;
    }
}

/* Prints a letter C that is n characters wide and n characters tall.
 *
 * For example, printC(4) prints
 *
 *       ****
 *       *
 *       *
 *       ****
 *
 * and printC(10) prints
 *
 *       **********
 *       *
 *       *
 *       *
 *       *
 *       *
 *       *
 *       *
 *       *
 *       **********
 */
void printC(int n) {
    for (int row = 0; row < n; row++) {
        /* First/last row print a row of stars. */
        if (row == 0 || row == n - 1) {
            for (int col = 0; col < n; col++) {
                cout << '*';
            }
            cout << endl;
        }
        /* Every othe row prints just a single star. */
        else {
            cout << '*' << endl;
        }
    }
}

/* Prints a letter I that's n characters wide and n characters
 * tall. For example, calling printI(5) prints
 *
 *       *****
 *         *
 *         *
 *         *
 *       *****
 *
 * and calling printI(7) prints
 *
 *       *******
 *          *
 *          *
 *          *
 *          *
 *          *
 *       *******
 */
void printI(int n) {
    for (int row = 0; row < n; row++) {
        /* Top and bottom rows are full of stars. */
        if (row == 0 || row == n - 1) {
            for (int col = 0; col < n; col++) {
                cout << '*';
            }
        }
        /* Every other row needs a star at the halfway point. */
        else {
            for (int col = 0; col < (n / 2); col++) {
                cout << ' ';
            }
            cout << '*';
        }
        cout << endl;
    }
}

/* Prints a letter P that's n characters wide and n characters
 * tall. For example, calling printP(5) prints
 *
 *       *****
 *       *   *
 *       *****
 *       *
 *       *
 *
 * and calling printP(7) prints
 *
 *       *******
 *       *     *
 *       *     *
 *       *******
 *       *
 *       *
 *       *
 */
void printP(int n) {
    for (int row = 0; row < n; row++) {
        /* Top and middle rows are full of stars. */
        if (row == 0 || row == n / 2) {
            for (int col = 0; col < n; col++) {
                cout << '*';
            }
        }
        /* Every other row in the top half has a star at the front and back. */
        else if (row < n / 2) {
            for (int col = 0; col < n; col++) {
                if (col == 0 || col == n - 1) {
                    cout << '*';
                } else {
                    cout << ' ';
                }
            }
        }
        /* All remaining rows just have a star at the front. */
        else {
            cout << '*';
        }
        cout << endl;
    }
}

/* Prints the word "CHIP" using letters of the given size. */
void printChip(int n) {
    printC(n);
    cout << endl;
    printH(n);
    cout << endl;
    printI(n);
    cout << endl;
    printP(n);
}

/******************************************************************************
 ******************************************************************************
 *********                 Part Two: Counting Triples                 *********
 ******************************************************************************
 ******************************************************************************/

/* Given a vector of integers, counts the number of triples of elements in
 * the vector that sum up to exactly the target. For example, given the
 * input {0, 1, 2, 3, 4} and a target of 5, the function would return 2,
 * since there are two triples of numbers ({0, 1, 4} and {0, 2, 3})
 */
int countTriples(const Vector<int>& values, int target) {
    int result = 0;
    for (int i = 0; i < values.size(); i++) {
        for (int j = i + 1; j < values.size(); j++) {
            for (int k = j + i; k < values.size(); k++) {
                if (values[i] + values[j] + values[k] == target) {
                    result++;
                }
            }
        }
    }
    return result;
}

/******************************************************************************
 ******************************************************************************
 *********                Part Three: Printing Cycles                 *********
 ******************************************************************************
 ******************************************************************************/

/* Given a vector of integers, cycles through the vector by repeatedly
 * printing out the first element, then removing it and moving it to
 * the back of the vector. This is similar to the Looper example from
 * class, except that it only does one loop.
 *
 * This first version uses .remove(0) and += to do the cycling.
 */
void printCycle_v1(Vector<int>& values) {
    for (int i = 0; i < values.size(); i++) {
        int first = values[0];
        values.remove(0); // Remove first element
        values += first;  // Append that element to the back.

        cout << first << endl;
    }
}

/* Given a vector of integers, cycles through the vector by repeatedly
 * printing out the first element, then removing it and moving it to
 * the back of the vector. This is similar to the Looper example from
 * class, except that it only does one loop.
 *
 * This first version uses .subList and + to do the cycling.
 */
void printCycle_v2(Vector<int>& values) {
    for (int i = 0; i < values.size(); i++) {
        int first = values[0];
        values = values.subList(1) + first; // Cycle the elements

        cout << first << endl;
    }
}

/* Given a queue of integers, cycles through the queue using
 * an approach similar to the one we used for the looper.
 */
void printCycle_v3(Queue<int>& values) {
    for (int i = 0; i < values.size(); i++) {
        int first = values.dequeue();
        values.enqueue(first);

        cout << first << endl;
    }
}

/******************************************************************************
 ******************************************************************************
 *********                Part Four: Recursive Puzzles                *********
 ******************************************************************************
 ******************************************************************************/

/* A recursive function designed to give you practice assessing efficiency.
 * Trace out how the recursion works and use that to form a hypothesis about
 * the runtime.
 */
int recursivePuzzle(int n) {
    if (n == 0) {
        return 1;
    }
    return 1 + recursivePuzzle(n - 1);
}

/* A recursive function designed to give you practice assessing efficiency.
 * Trace out how the recursion works and use that to form a hypothesis about
 * the runtime.
 */
int recursiveEnigma(int n) {
    if (n == 0) {
        return 1;
    }
    return 1 + recursiveEnigma(n / 2);
}

/******************************************************************************
 ******************************************************************************
 *********            Part Five: Maximum Single-Sell Profit           *********
 ******************************************************************************
 ******************************************************************************/

/* The "Maximum Single-Sell Profit" problem. In this problem, you're given a list of
 * the historical values of a stock. You're allowed to buy the stock once and sell it
 * once. The goal is to make the maximum amount of profit possible. For example, suppose
 * you're given this price list:
 *
 *         5 6 7 1 2 3 4
 *
 * Here, the best option would be to buy the stock when it's retailing for $1, then
 * sell it when it's valued at $4. We can't buy it at $1 and sell it for $7 because
 * the prices reflect the historical value of the stock in order; the stock was at
 * $7 but then dropped to $1 immediately afterwards.
 *
 * Or suppose you have this list of numbers:
 *
 *        10 9 8 7 6 5 4 3 2 1
 *
 * In this case, your maximum profit would be $0: buy the stock and then sell it
 * immediately.
 *
 * There are many ways to solve this problem. This first version of the function works
 * by looking at all buy/sell pairs and taking the best.
 */
int maximumSingleSellProfit_v1(const Vector<int>& values) {
    int best = 0; // Could buy and sell on the same day for $0 profit.
    for (int i = 0; i < values.size(); i++) {
        for (int j = i + 1; j < values.size(); j++) {
            best = max(best, values[j] - values[i]);
        }
    }
    return best;
}

/* There's a very clever recursive algorithm for solving this problem. There's some
 * time we buy the stock and some time we sell the stock. Those times are either
 *
 *   1. purely in the first half of the array,
 *   2. purely in the second half of the array, or
 *   3. the purchase time is in the first half and the sell time is in the second.
 *
 * We can find the best solution to (1) or (2) using recursion - finding the best
 * profit we can make on a smaller subarray. For (3), we take the minimum price
 * in the first half and the maximum price in the second half. The best option
 * must be one of those three.
 */
int maximumSingleSellProfit_v2(const Vector<int>& values) {
    /* Base case: If the list has fewer than two elements, then you either can't
     * buy or sell anything or you have to sell at the buy price. Either way you
     * make no profit.
     */
    if (values.size() < 2) {
        return 0;
    }

    int half = values.size() / 2;

    /* Find the max profit buying and selling purely in the first or
     * second half of the list.
     */
    int firstHalfProfit  = maximumSingleSellProfit_v2(values.subList(0, half));
    int secondHalfProfit = maximumSingleSellProfit_v2(values.subList(half));

    /* Find the best buy price in the first half and theb est buy price in the
     * second half.
     */
    int firstMin = values[0];
    for (int i = 1; i < half; i++) {
        firstMin = min(firstMin, values[i]);
    }
    int secondMax = values[half];
    for (int i = half + 1; i < values.size(); i++) {
        secondMax = max(secondMax, values[i]);
    }

    /* The best option is either the max profit purely buying and selling in the
     * first half, or the max profit buying and selling purely in the second half,
     * or by buying in the first half and selling in the second.
     */
    return max({ firstHalfProfit, secondHalfProfit, secondMax - firstMin });
}
