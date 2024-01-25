#include <iostream>
#include "console.h"
#include "SimpleTest.h"
#include "strlib.h"
using namespace std;

string extractAlpha(string s)
{
    string result;

    for (int i = 0; i < s.length(); i++)
    {
        if (isalpha(s[i]))
        {
            result += s[i];
        }
    }
    return result;
}

bool passwordChecksOut(string username, string password)
{
    string alphaPortion = extractAlpha(password);
    return !stringContains(toLowerCase(alphaPortion), toLowerCase(username));
}

STUDENT_TEST("various tests of passwordCheckOut() function") {
    EXPECT_EQUAL(passwordChecksOut("xiaokang", "666666XiaoKang"), false);
    EXPECT_EQUAL(passwordChecksOut("xiaokang", "666666kangxiao"), true);
    EXPECT_EQUAL(passwordChecksOut("sean", "s9e$a***&n"), false);
}

STUDENT_TEST("various tests of extractAlpha() function")
{
    EXPECT_EQUAL(extractAlpha("sean"), "sean");
    EXPECT_EQUAL(extractAlpha("sean11"), "sean");
    EXPECT_EQUAL(extractAlpha("1sean"), "sean");
    EXPECT_EQUAL(extractAlpha("s1ean"), "sean");
    EXPECT_EQUAL(extractAlpha("s1e1a1n"), "sean");
    EXPECT_EQUAL(extractAlpha("s9e$a***&n"), "sean");
    EXPECT_EQUAL(extractAlpha(""), "");
    EXPECT_EQUAL(extractAlpha("9$&"), "");
}

int main_()
{
    // runSimpleTests(ALL_TESTS);

    string username = "Sean";
    string password = "s1e1a1n1'1s1p1a1s1s1w1o1r1d";

    if (passwordChecksOut(username, password))
    {
        cout << "Hooray!" << endl;
    }
    else
    {
        cout << "Oh no! Bad password." << endl;
    }

    return 0;
}
