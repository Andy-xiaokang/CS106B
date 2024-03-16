#include <iostream>
#include <cmath>
#include "console.h"
#include "SimpleTest.h"
#include "vector.h"
using namespace std;

int getWidth(int num) {
    int result = 0;
    if (num == 0) return 1;
    while (num != 0) {
        num = num / 10;
        result++;
    }
    return result;
}

int getMaxWidth(Vector<int>& v) {
    int result = 0;
    for (int num : v) {
        int width = getWidth(num);
        if (result < width) result = width;
    }
    return result;
}

void printHence(int maxWidth) {
    cout << '+';
    for (int i = 0; i < maxWidth; i++) {
        cout << '-';
    }
}

void printSpace(int width) {
    for (int i = 0; i < width; i++) {
        cout << ' ';
    }
}

void printVector(Vector<int>& v) {
    int maxWidth = getMaxWidth(v);
    int size = v.size();
    // print line 1
    for (int i = 0; i < size; i++) {
        printHence(maxWidth + 2);
    }
    cout << '+' << endl;
    // print line 2
    for (int num : v) {
        int leftSpaceNum = floor((maxWidth + 2 - getWidth(num)) / 2.0);
        int rightSpaceNum = ceil((maxWidth + 2 - getWidth(num)) / 2.0);
        cout << '|';
        printSpace(leftSpaceNum);
        cout << num;
        printSpace(rightSpaceNum);
    }
    cout << '|' << endl;
    // print line 3
    for (int i = 0; i < size; i++) {
        printHence(maxWidth + 2);
    }
    cout << '+' << endl;
    // print line 4
    for (int i = 0; i < size; i++) {
        int leftSpaceNum = floor((maxWidth + 2 - getWidth(i)) / 2.0);
        int rightSpaceNum = ceil((maxWidth + 2 - getWidth(i)) / 2.0);
        printSpace(leftSpaceNum + 1);
        cout << i;
        printSpace(rightSpaceNum);
    }
    cout << ' ' << endl;
}

STUDENT_TEST("test for getWidth function") {
    EXPECT_EQUAL(getWidth(10), 2);
    Vector<int> v = {43, 1, 9, 129, 5821, 3};
    EXPECT_EQUAL(getMaxWidth(v), 4);
}

int main() {
    // runSimpleTests(ALL_TESTS);
    Vector<int> v = {43, 1, 9, 129, 5821, 3};
    Vector<int> p = {2, 0, 1, 2345, 8923348, 345, 6735, 8942, 23};
    printVector(v);
    printVector(p);
    return 0;
}
