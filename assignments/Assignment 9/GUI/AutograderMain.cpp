#include "Core.h"
#include "SimpleTest.h"
#include "TestingGUI.h"
#include "AutograderMain.h"
#include "qtgui.h"
using namespace std;

void MiniGUI::Detail::autograderMain() {
    int argc = QtGui::instance()->getArgc();
    char** argv = QtGui::instance()->getArgv();

    /* Need to know what to run. */
    if (argc == 1) {
        cerr << "Usage: autograder test-filename" << endl;
        _Exit(-1);
    }

    string testFileName = argv[1];

    /* Run all tests in console mode, only including autograder tests. */
    runConsoleModeTests([testFileName](const string& filename, const SimpleTest::Test& test) {
        return test.type == SimpleTest::TestType::AUTOGRADER &&
               filename == testFileName;
    }, true);

    /* Hard abort; we're done. */
    _Exit(0);
}
