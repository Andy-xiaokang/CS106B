#include <iostream>
#include "console.h"
#include "SimpleTest.h"

using namespace std;


int main_() {
    if (runSimpleTests(SELECTED_TESTS)) {
        return 0;
    }

    return 0;
}
