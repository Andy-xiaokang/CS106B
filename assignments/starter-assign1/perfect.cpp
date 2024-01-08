/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include "console.h"
#include <iostream>
#include "SimpleTest.h"
#include "perfect.h"
using namespace std;

/* The divisorSum function takes one argument `n` and calculates the
 * sum of proper divisors of `n` excluding itself. To find divisors
 * a loop iterates over all numbers from 1 to n-1, testing for a
 * zero remainder from the division using the modulus operator %
 *
 * Note: the C++ long type is a variant of int that allows for a
 * larger range of values. For all intents and purposes, you can
 * treat it like you would an int.
 */
long divisorSum(long n) {
    long total = 0;
    for (long divisor = 1; divisor < n; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
        }
    }
    return total;
}

/* The isPerfect function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself.
 */
bool isPerfect(long n) {
    return (n != 0) && (n == divisorSum(n));
}

/* The findPerfects function takes one argument `stop` and performs
 * an exhaustive search for perfect numbers over the range 1 to `stop`.
 * Each perfect number found is printed to the console.
 */
void findPerfects(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfect(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* The smarterSum function takes one argument `n` and returns its
 * divisor sum.
 */
long smarterSum(long n) {
    long total = 0;
    double squareRoot = sqrt(n);
    if(n == 1) return 0;
    for (int divisor = 1; divisor < squareRoot; divisor++) {
        if (n % divisor == 0) {
            total += (divisor + n / divisor);
        }
    }
    if  (floor(squareRoot) == ceil(squareRoot)) return total - n + (long) squareRoot;
    return total - n;
}

/* take a parameter n and return is n a perfect number.
 */
bool isPerfectSmarter(long n) {
    return (n != 0) && (n == smarterSum(n));
}

/* take a parameter stop and print all perfect number between 1 to stop - 1;
 */
void findPerfectsSmarter(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfectSmarter(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* the findNthPerfectEuclid function take one argument n
 * and returns the nth perfect number.
 */
long findNthPerfectEuclid(long n) {
    int m = 0;
    for (int k = 1; ; k++) {
        m = pow(2, k) - 1;
        if (smarterSum(m) == 1) {
            n--;
            if (n == 0) {
                return pow(2, k - 1) * (pow(2, k) -  1);
            }
        }
    }
}

/* * * * * * Test Cases * * * * * */

// TODO: add your STUDENT_TEST test cases here!
// STUDENT_TEST("find largest size complete around 60s") {
//     TIME_OPERATION(100000, findPerfects(100000));
// }
STUDENT_TEST("test finsNthPerfectEuclid function for n") {
    EXPECT_EQUAL(findNthPerfectEuclid(1), 6);
    EXPECT_EQUAL(findNthPerfectEuclid(2), 28);
    EXPECT_EQUAL(findNthPerfectEuclid(3), 496);
    EXPECT_EQUAL(findNthPerfectEuclid(4), 8128);
    EXPECT_EQUAL(findNthPerfectEuclid(5), 33550336);
}

STUDENT_TEST("test time for finsNth PerfectEuclid function") {
    TIME_OPERATION(5, findNthPerfectEuclid(5));
    TIME_OPERATION(6, findNthPerfectEuclid(6));
}

STUDENT_TEST("test isPerfect for negative number") {
    EXPECT(!isPerfect(-1));
    EXPECT(!isPerfect(-2));
}

STUDENT_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(smarterSum(1), 0);
    EXPECT_EQUAL(smarterSum(6), 6);
    EXPECT_EQUAL(smarterSum(12), 16);
    EXPECT_EQUAL(smarterSum(28), 28);
    EXPECT_EQUAL(smarterSum(25), 6);
    EXPECT_EQUAL(smarterSum(8128), 8128);
}

/*
 * Here is sample test demonstrating how to use a loop to set the input sizes
 * for a sequence of time trials.
 */
STUDENT_TEST("Multiple time trials of findPerfects on increasing input sizes") {
   int smallest = 1000, largest = 8000;
   for (int size = smallest; size <= largest; size *= 2) {
       TIME_OPERATION(size, findPerfects(size));
   }
}

STUDENT_TEST("Multiple time trials of findPerfects on increasing input sizes") {
    int smallest = 1000, largest = 64000;
    for (int size = smallest; size <= largest; size *= 2) {
        TIME_OPERATION(size, findPerfectsSmarter(size));
    }
}

/* Please not add/modify/remove the PROVIDED_TEST entries below.
 * Place your student tests cases above the provided tests.
 */

PROVIDED_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(divisorSum(1), 0);
    EXPECT_EQUAL(divisorSum(6), 6);
    EXPECT_EQUAL(divisorSum(12), 16);
}

PROVIDED_TEST("Confirm 6 and 28 are perfect") {
    EXPECT(isPerfect(6));
    EXPECT(isPerfect(28));
}

PROVIDED_TEST("Confirm 12 and 98765 are not perfect") {
    EXPECT(!isPerfect(12));
    EXPECT(!isPerfect(98765));
}

PROVIDED_TEST("Test oddballs: 0 and 1 are not perfect") {
    EXPECT(!isPerfect(0));
    EXPECT(!isPerfect(1));
}

PROVIDED_TEST("Confirm 33550336 is perfect") {
    EXPECT(isPerfect(33550336));
}

PROVIDED_TEST("Time trial of findPerfects on input size 1000") {
    TIME_OPERATION(1000, findPerfects(1000));
}
