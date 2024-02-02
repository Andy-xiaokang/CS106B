#include <iostream>
#include "console.h"
#include "SimpleTest.h"
#include "stack.h"
#include "vector.h"
#include "strlib.h"
#include "map.h"
using namespace std;

bool processPostfix(string expr, int& result) {
    Vector<string> tokens = stringSplit(expr, ' ');
    Stack<int> stack;
    int rightOprand, leftOprand;
    for (string token : tokens) {
        if (stringIsInteger(token)) {
            stack.push(stringToInteger(token));
        } else if (token == "+" && stack.size() >= 2) {
            rightOprand = stack.pop();
            leftOprand = stack.pop();
            stack.push(leftOprand + rightOprand);
        } else if (token == "-" && stack.size() >= 2) {
            rightOprand = stack.pop();
            leftOprand = stack.pop();
            stack.push(leftOprand - rightOprand);
        } else if (token == "*" && stack.size() >= 2) {
            rightOprand = stack.pop();
            leftOprand = stack.pop();
            stack.push(leftOprand * rightOprand);
        } else if (token == "/" && stack.size() >= 2) {
            rightOprand = stack.pop();
            if (rightOprand == 0) return false;
            leftOprand = stack.pop();
            stack.push(leftOprand / rightOprand);
        } else {
            return false;
        }
    }
    if (stack.size() == 1) {
        result = stack.pop();
        return true;
    } else {
        return false;
    }
}

PROVIDED_TEST("valid postfix expressions")
{
    int result = 0;

    EXPECT_EQUAL(processPostfix("5 10 +", result), true);
    EXPECT_EQUAL(result, 15);

    EXPECT_EQUAL(processPostfix("3 8 *", result), true);
    EXPECT_EQUAL(result, 24);

    EXPECT_EQUAL(processPostfix("5 10 12 + -", result), true);
    EXPECT_EQUAL(result, -17);

    EXPECT_EQUAL(processPostfix("10 12 + 5 -", result), true);
    EXPECT_EQUAL(result, 17);

    EXPECT_EQUAL(processPostfix("5 2 * 3 + 4 -", result), true);
    EXPECT_EQUAL(result, 9);

    EXPECT_EQUAL(processPostfix("2 3 + 4 5 + *", result), true);
    EXPECT_EQUAL(result, 45);

    EXPECT_EQUAL(processPostfix("2 3 4 * + 5 +", result), true);
    EXPECT_EQUAL(result, 19);

    EXPECT_EQUAL(processPostfix("2 3 + 4 +", result), true);
    EXPECT_EQUAL(result, 9);

    EXPECT_EQUAL(processPostfix("2 3 4 + +", result), true);
    EXPECT_EQUAL(result, 9);

    EXPECT_EQUAL(processPostfix("2 3 1 * + 9 -", result), true);
    EXPECT_EQUAL(result, -4);

    EXPECT_EQUAL(processPostfix("10", result), true);
    EXPECT_EQUAL(result, 10);
}

PROVIDED_TEST("invalid postfix expressions")
{
    int result = 0;

    EXPECT_EQUAL(processPostfix("5 10 + +", --result), false);
    EXPECT_EQUAL(result, -1);

    EXPECT_EQUAL(processPostfix("3 8 * 0 / 5 +", --result), false);
    EXPECT_EQUAL(result, -2);

    EXPECT_EQUAL(processPostfix("", --result), false);
    EXPECT_EQUAL(result, -3);

    EXPECT_EQUAL(processPostfix("10 12 + 13", --result), false);
    EXPECT_EQUAL(result, -4);

    EXPECT_EQUAL(processPostfix("10 12 13 +", --result), false);
    EXPECT_EQUAL(result, -5);

    EXPECT_EQUAL(processPostfix("10 + 20", --result), false);
    EXPECT_EQUAL(result, -6);

    EXPECT_EQUAL(processPostfix("- 10 8", --result), false);
    EXPECT_EQUAL(result, -7);

    EXPECT_EQUAL(processPostfix("- 10", --result), false);
    EXPECT_EQUAL(result, -8);

    EXPECT_EQUAL(processPostfix("-", --result), false);
    EXPECT_EQUAL(result, -9);

    EXPECT_EQUAL(processPostfix("10 15 + sandwhich", --result), false);
    EXPECT_EQUAL(result, -10);
}

int main()
{
    runSimpleTests(ALL_TESTS);
    Vector<int> numbers = {1, 2, 3, 4, 5};
    for (int& num : numbers) {
        num += 2;
    }
    cout << numbers << endl;

    Map<string, Vector<string>> map;
    Vector<string> juliaName = map["julia"];
    juliaName.add("zelensiki");
    juliaName.add("standford");
    Vector<string>& chrisNames = map["Chris"];
    chrisNames.add("Gregg");
    chrisNames.add("Piech");
    cout << map << endl;
    cout << "map size " << map.size() << endl;
    return 0;
}
