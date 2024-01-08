/*
 * CS106B Section Handout Test Harness: Section 1
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified by Trip Master and Nick
 * Bowman for CS106B Spring 2020.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */
#include <iostream>
#include "SimpleTest.h"
#include "vector.h"
#include "simpio.h"
#include "strlib.h"
#include "filelib.h"
#include <fstream>
using namespace std;

/*
 * Count Numbers (Code Write)
 * ----------------------------------
 * Write a program to read through a given file and count the
 * the number of times a user inputed number appears in that file. You  
 * can assume that numbers will be composed entirely of numerical digits, 
 * optionally preceded by a single negative sign.
 */
void countNumbers(string filepath) {
    ifstream in;
    if (!openFile(in, filepath)) {
        return;
    }
    Vector<string> lines = readLines(in);
    cout << lines << endl;
    while (true) {
        string number = getLine("please enter your number: ");
        int count = 0;
        if (number == "") {
            break;
        }
        if (!stringIsInteger(number)) {
            continue;
        }
        for (string line : lines) {
            Vector<string> tokens = stringSplit(line, " ");
            for (string token : tokens) {
                if (token == number) {
                    count++;
                }
            }
        }
        cout << "Number " << number << " appeared " << count << " times in the file." << endl;
    }
}

int main_count() {
    countNumbers("res/numbers.txt");
    return 0;
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Count numbers") {
    /* This isn't a full on test, since the results of countNumbers
     * depends on user input. To confirm your solution works, you'd need
    to manually check the print statements in the console. */
    EXPECT_NO_ERROR(countNumbers("res/numbers.txt"));
}

