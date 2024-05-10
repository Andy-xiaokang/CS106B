/*
 * CS106B Section Handout Test Harness: Section 1
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified for Qt and Ed by Neel
 * Kishnani for Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include <iostream>
#include <fstream>
#include <string>
#include "testing/SimpleTest.h"
using namespace std;

struct Statistics {
    double min;     // Smallest value in the file
    double max;     // Largest value in the file
    double average; // Average value in the file
};

/*
 * Function: documentStatisticsFor
 * -------------------------------
 * Reads in a file of numbers via istream and calculates statistics on
 * it (i.e. min, max, average). Represents stats in form of struct 
 * defined above.
 */
Statistics documentStatisticsFor(istream& input) {
    // double val;
    // while (input >> val) {

    // }
    Statistics result;
    double min = 0, max = 0, average = 0, sum = 0;
    int count = 0;
    for (double val; input >> val;) {
        sum += val;
        if (val < min) {
            min =  val;
        }
        if (val > max) {
            max = val;
        }
        count++;
    }
    result.min = min;
    result.max = max;
    result.average = sum / count;
    return result;
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Provided Test: Use sample text file to compute statistics.") {
    ifstream input;
    input.open("res/stats.txt");
    Statistics s = documentStatisticsFor(input);
    EXPECT_EQUAL(s.min, -4.2);
    EXPECT_EQUAL(s.max, 30.12);
    EXPECT_EQUAL(s.average, 5.802);
}
