#include "TestingGUI.h"
#include "ProblemHandler.h"
#include "Core.h"
#include "TemporaryComponent.h"
#include "GColorConsole.h"
#include "GUIMain.h"
#include "TextUtils.h"
#include "MemoryDiagnostics.h"
#include "SimpleTest.h"
#include "TestDriver.h"
#include "error.h"
#include "simpio.h"
#include "gthread.h"
using namespace std;

namespace {
    /* Test type --> String */
    string to_string(SimpleTest::TestType type) {
        switch (type) {
            case SimpleTest::TestType::STUDENT: return "Student Test";
            case SimpleTest::TestType::PROVIDED: return "Provided Test";
            case SimpleTest::TestType::AUTOGRADER: return "Autograder Test";
            case SimpleTest::TestType::MANUAL: return "Manual Test";
            default: error("Unknown test type.");
        }
    }

    /* Test --> Display Name */
    string displayNameOf(const SimpleTest::Test& test) {
        return to_string(test.type) + ": " + test.name;
    }

    /* Given a file path, returns the name of the file, excluding the path to it. */
    string tailOf(const string& path) {
        /* Find the last / or \ in the filename. */
        size_t index = path.find_last_of("\\/");
        return index == string::npos ? path : path.substr(index + 1);
    }

    /* Everything is allowed. */
    SimpleTest::TestFilter filter() {
        return [](const string&, const SimpleTest::Test&) {
            return true;
        };
    }

    SimpleTest::TestGroupComparator comparator() {
        /* Sort files by their file index. */
        auto fileList = MiniGUI::Config::testOrder();

        return [=](const string& lhs, const string& rhs) {
            /* Compare indices of the two strings. */
            return find(fileList.begin(), fileList.end(), lhs) <
                   find(fileList.begin(), fileList.end(), rhs);
        };
    }

    /* Retrieves all files that contain tests, i.e. all test groups */
    Vector<string> getTestGroups() {
        Vector<string> keys;
        auto& tests = SimpleTest::Internal::rawTests();

        for (const auto& entry: tests) {
            keys += tailOf(entry.first);
        }

        /* Sort by the canonical ordering. */
        auto fileList = MiniGUI::Config::testOrder();
        sort(keys.begin(), keys.end(), [&](const string& lhs, const string& rhs) {
            return find(fileList.begin(), fileList.end(), lhs) <
                   find(fileList.begin(), fileList.end(), rhs);
        });

        return keys;
    }

    /* Prompts the user via the console for which test group to run (or all of them). -1 -> all. */
    int getTestSelection(const Vector<string>& groups) {
        cout << "Select which test to run!" << endl;

        cout << "0 All Tests" << endl;
        for (int i = 0; i < groups.size(); i++) {
            cout << i + 1 << " " << groups[i] << endl;
        }

        return getInteger("Select which test to run: ") - 1;
    }

    /* Returns an appropriate TestFilter given the Vector of test groups. */
    SimpleTest::TestFilter filterToSelection(const Vector<string>& groups, int selection) {
        if (selection < 0) {
            return filter();
        }

        string selectedGroup = groups[selection];
        auto defaultFilter = filter();

        return [selectedGroup, defaultFilter](const string& group, const SimpleTest::Test& test) {
            return group == selectedGroup && defaultFilter(group, test);
        };
    }

    /* Displays all the results from the given test group. */
    void displayResults(GColorConsole& console, const Vector<SimpleTest::TestGroup>& testGroups) {
        /* Do on the main GUI thread so that we can always see where we are. */
        GThread::runOnQtGuiThread([&] {
            console.clearDisplay();

            for (const auto& group: testGroups) {
                /* Draw Header */
                console.doWithStyle("#000080", GColorConsole::BOLD, [&] {
                    console << "==== Tests for " << group.name << " ====" << '\n';
                });

                /* Draw each test */
                for (const auto& test: group.tests) {
                    if (test.result == SimpleTest::TestResult::WAITING) {
                        console.doWithStyle("#A0A0A0", GColorConsole::ITALIC, [&] {
                            console << "         " << displayNameOf(test) << '\n';
                        });
                    } else if (test.result == SimpleTest::TestResult::RUNNING) {
                        console.doWithStyle("#0000FF", GColorConsole::ITALIC, [&] {
                            console << "running: " << displayNameOf(test) << '\n';
                        });
                    } else if (test.result == SimpleTest::TestResult::PASS) {
                        console.doWithStyle("#404040", GColorConsole::ITALIC, [&] {
                            console << "   pass: " << displayNameOf(test) << '\n';
                        });
                    } else if (test.result == SimpleTest::TestResult::FAIL) {
                        console.doWithStyle("#800000", GColorConsole::BOLD, [&] {
                            console << "   FAIL: " << displayNameOf(test) << '\n';
                            console << "    " << test.detailMessage << '\n';
                        });
                    } else if (test.result == SimpleTest::TestResult::EXCEPTION) {
                        console.doWithStyle("#FF0000", GColorConsole::BOLD, [&] {
                            console << "   FAIL: " << displayNameOf(test) << '\n';
                            console << "    " << test.detailMessage << '\n';
                        });
                    } else if (test.result == SimpleTest::TestResult::LEAK) {
                        console.doWithStyle("#FF0000", GColorConsole::BOLD, [&] {
                            console << "   FAIL: " << displayNameOf(test) << '\n';
                            console << "    " << "Test defined on line " << test.lineNumber << "." << '\n';
                            console << "    " << test.detailMessage << '\n';
                        });
                    } else {
                        error("Internal error: Unknown test result. Contact the course staff.");
                    }
                }

                /* Draw the overall result. */
                if (group.numPassed == group.numTests && group.numTests > 0) {
                    console.doWithStyle("#0000FF", GColorConsole::ITALIC, [&] {
                        console << "All tests in this section passed!" << '\n';
                        console << '\n';
                        console << '\n';
                    });
                } else {
                    console.doWithStyle("#400000", GColorConsole::BOLD, [&] {
                        console << "Summary: " << group.numPassed << " / " << group.numTests
                             << " test" << (group.numTests == 1? "" : "s") << " passed." << '\n';
                        console << '\n';
                        console << '\n';
                    });
                }
            }

            console.flush();
        });
    }

    class TestingGUI: public ProblemHandler {
    public:
        TestingGUI(GWindow& window);

        void settingUp() override;

    private:
        Temporary<GColorConsole> console;
    };

    /* Initialize chrome. */
    TestingGUI::TestingGUI(GWindow& window) : ProblemHandler(window) {
        console = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
        setDemoOptionsEnabled(false);
    }

    /* Actually run all the tests. */
    void TestingGUI::settingUp() {
        SimpleTest::TestReporter reporter = [this](const Vector<SimpleTest::TestGroup>& groups) {
            displayResults(*console, groups);
        };

        SimpleTest::run(reporter, filter(), comparator());

        setDemoOptionsEnabled(true);
    }
}

GRAPHICS_HANDLER("Run Tests", GWindow& window) {
    return make_shared<TestingGUI>(window);
}

namespace {
    /* Type that diverts a stream from its normal destination
     * to another location. This can be used, for example, to
     * divert stdout or stderr.
     */
    class OstreamDiverter {
    public:
        explicit OstreamDiverter(ostream& out) : original(out), diverted(out.rdbuf(replacement.rdbuf())) {

        }

        ~OstreamDiverter() {
            original.rdbuf(diverted.rdbuf());
        }

        /* Access to write to the original source. */
        ostream& out() {
            return diverted;
        }

        /* What was written. */
        std::string written() {
            return replacement.str();
        }

    private:
        ostream& original;
        ostringstream replacement;
        ostream  diverted;
    };

    /* Runs all the tests in console mode. */
    void runConsoleTests(SimpleTest::TestFilter filter, bool divertStreams) {
        Vector<SimpleTest::TestGroup> lastGroups;

        {
            /* Divert the stream if we're in autograder mode. */
            unique_ptr<OstreamDiverter> newCout, newCerr;
            if (divertStreams) {
                newCout.reset(new OstreamDiverter(cout));
                newCerr.reset(new OstreamDiverter(cerr));
            }

            const SimpleTest::Test* running = nullptr;
            SimpleTest::TestReporter reporter = [&](const Vector<SimpleTest::TestGroup>& groups) {
                /* cout may be diverted. If so, use the "real" cout. */
                ostream& out = newCout? newCout->out() : cout;

                /* Stash the last set of groups so that we can print failures later. */
                lastGroups = groups;
                for (const auto& group: groups) {
                    for (const auto& test: group.tests) {
                        if (running == &test) {
                            if (test.result == SimpleTest::TestResult::PASS) {
                                out << "    pass" << endl;
                            } else if (test.result == SimpleTest::TestResult::FAIL) {
                                out << "    FAIL: " << test.detailMessage << endl;
                            } else if (test.result == SimpleTest::TestResult::EXCEPTION) {
                                out << "    FAIL: " << test.detailMessage << endl;
                            } else if (test.result == SimpleTest::TestResult::LEAK) {
                                out << "    LEAK: " << test.detailMessage << endl;
                            } else {
                                error("Internal error: Unknown test result?");
                            }
                            running = nullptr;
                        }
                        if (test.result == SimpleTest::TestResult::RUNNING) {
                            running = &test;
                            out << "Running " << displayNameOf(test) << " from " << group.name << "." << endl;
                        }
                    }
                }
            };

            SimpleTest::run(reporter, filter, comparator());
        }

        cout << endl;
        cout << "Test summary: " << endl;

        /* List all errors. */
        int totalTests = 0, totalPassed = 0;
        for (const auto& group: lastGroups) {
            totalTests  += group.numTests;
            totalPassed += group.numPassed;

            if (group.numPassed != group.numTests) {
                cerr << "Tests failed in " << group.name << ":" << endl;

                for (const auto& test: group.tests) {
                    if (test.result != SimpleTest::TestResult::PASS) {
                        cerr << "FAIL: " << test.name << " (line " << test.lineNumber << ")" << endl;
                        cerr << test.detailMessage << endl;
                    }
                }
            }
        }

        /* List error counts. */
        for (const auto& group: lastGroups) {
            cout << group.name << ": " << group.numPassed << " of " << pluralize(group.numTests, "test") << " passed." << endl;
        }

        if (lastGroups.size() > 1) {
            cout << "Overall: " << totalPassed << " of " << pluralize(totalTests, "test") << " passed." << endl;
        }

        if (totalTests == totalPassed) {
            cout << "All tests passed!" << endl;
        }
    }
}

namespace MiniGUI {
    namespace Detail {
        void runConsoleModeTests(SimpleTest::TestFilter filter, bool divertStreams) {
            runConsoleTests(filter, divertStreams);
        }
    }
}

CONSOLE_HANDLER("Run Tests") {
    auto groups = getTestGroups();
    int selection = getTestSelection(groups);

    runConsoleTests(filterToSelection(groups, selection), false);
}
