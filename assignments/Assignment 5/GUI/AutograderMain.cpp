#include "Core.h"
#include "SimpleTest.h"
#include "TestingGUI.h"
#include "AutograderMain.h"
using namespace std;

void MiniGUI::Detail::autograderMain() {
    /* Run all tests in console mode, only including autograder tests. */
    runConsoleModeTests([](const string&, const SimpleTest::Test& test) {
        return test.type == SimpleTest::TestType::AUTOGRADER;
    }, true);

    /* Hard abort; we're done. */
    _Exit(0);
}
