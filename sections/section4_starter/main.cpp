#include <iostream>
#include "console.h"
#include "testing/SimpleTest.h"

using namespace std;


int main() 
{
    if (runSimpleTests(SELECTED_TESTS)) {
        return 0;
    }
    return 0;
}
