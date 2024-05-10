#include <iostream>
#include "console.h"
#include "testing/SimpleTest.h"

using namespace std;


int main() 
{
    char c = 'k';
    cout << to_string(c) + "ang";
    if (runSimpleTests(SELECTED_TESTS)) {
        return 0;
    }

    return 0;
}
