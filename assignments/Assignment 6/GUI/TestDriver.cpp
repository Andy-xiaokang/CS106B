#include "TestDriver.h"
#include "SimpleTest.h"
#include "MemoryDiagnostics.h"
using namespace std;

namespace {
    /* Given a file path, returns the name of the file, excluding the path to it. */
    string tailOf(const string& path) {
        /* Find the last / or \ in the filename. */
        size_t index = path.find_last_of("\\/");
        return index == string::npos? path : path.substr(index + 1);
    }

    /* Runs a single test. */
    void runSingleTest(SimpleTest::Test& test, SimpleTest::TestGroup& group) {
        try {
            /* Reset memory counters so we don't have carryover across tests. */
            MemoryDiagnostics::clear();

            /* Run the test. */
            test.callback();

            /* See if there were any memory leaks. */
            auto errors = MemoryDiagnostics::typesWithErrors();
            if (errors.empty()) {
                test.result = SimpleTest::TestResult::PASS;
                group.numPassed++;
            } else {
                /* We have memory leaks. */
                test.result = SimpleTest::TestResult::LEAK;

                ostringstream out;
                out << "    Test failed due to memory errors with these types:" << endl;
                for (const auto& entry: errors) {
                    string type = entry.first;
                    int    delta = entry.second;

                    if (delta > 0) {
                        out << "            " << type << ": Leaked " << pluralize(delta, "object") << "." << endl;
                    } else {
                        out << "            " << type << ": Deallocated " << pluralize(-delta, "more object") << " than allocated." << endl;
                    }
                }

                test.detailMessage = out.str();
            }
        } catch (const SimpleTest::Internal::TestFailedException& e) {
            test.result = SimpleTest::TestResult::FAIL;
            ostringstream out;
            out << "    " << e.what() << endl;
            test.detailMessage = out.str();
        } catch (const ErrorException& e) {
            test.result = SimpleTest::TestResult::EXCEPTION;
            ostringstream out;
            out << "    Test failed due to the program triggering an ErrorException." << endl;
            out << endl;
            out << "    This means that the test did not fail because of a call" << endl;
            out << "    to EXPECT() or EXPECT_ERROR() failing, but rather because" << endl;
            out << "    some code explicitly called the error() function." << endl;
            out << endl;
            out << "    Error: " << e.getMessage() << endl;
            test.detailMessage = out.str();
        } catch (const exception& e) {
            test.result = SimpleTest::TestResult::EXCEPTION;
            ostringstream out;
            out << "    Test failed due to the program triggering an exception." << endl;
            out << endl;
            out << "    This means that the test did not fail because of a call" << endl;
            out << "    to EXPECT() or an EXPECT_ERROR() failing, but rather because" << endl;
            out << "    some code - probably an internal C++ library - triggered" << endl;
            out << "    an error." << endl;
            out << endl;
            out << "    Error: " << e.what() << endl;
            test.detailMessage = out.str();
        } catch (...) {
            test.result = SimpleTest::TestResult::EXCEPTION;
            ostringstream out;
            out << "    Test failed due to the program triggering an unknown type" << endl;
            out << "    of exception. " << endl;
            out << endl;
            out << "    This means that the test did not fail because of a call" << endl;
            out << "    to EXPECT() or an EXPECT_ERROR() failing, but rather because" << endl;
            out << "    some code triggered an error whose format we couldn't" << endl;
            out << "    recognize." << endl;
            out << endl;
            test.detailMessage = out.str();
        }
    }

    /* Given raw data from the testing system, produce a visual TestGroup for that data. */
    SimpleTest::TestGroup testsToGroup(const pair<const SimpleTest::Internal::TestKey, multimap<int, SimpleTest::Internal::TestCase>>& entry,
                                       SimpleTest::TestFilter filter) {
        /* Convert the raw testing group into a proper TestGroup object. */
        SimpleTest::TestGroup result;

        /* The test group's name is the full path to the file. We just want the tail. */
        result.name = tailOf(entry.first);
        result.numPassed = 0;
        result.numTests = entry.second.size();

        for (const auto& rawTest: entry.second) {
            SimpleTest::Test test;
            test.name = rawTest.second.name;
            test.type = rawTest.second.type;
            test.lineNumber = rawTest.second.lineNumber;
            test.result = SimpleTest::TestResult::WAITING; // It hasn't run yet
            test.callback = rawTest.second.callback;

            /* Exclude the test if it's not supposed to be run. */
            if (filter(result.name, test)) {
                result.tests += test;
            }
        }

        return result;
    }

    /* Default filter and comparator. */
    SimpleTest::TestFilter defaultFilter() {
        return [](const string&, const SimpleTest::Test&) {
            return true;
        };
    }
    SimpleTest::TestGroupComparator defaultComparator() {
        return std::less<std::string>();
    }
}

namespace SimpleTest{
    /* Runs the specified tests, updating the display as things progress. */
    void run(TestReporter reporter, TestFilter filter, TestGroupComparator comparator) {
        /* Convert from raw tests to a proper list of test groups. */
        Vector<TestGroup> displayedTests;
        for (const auto& entry: Internal::rawTests()) {
            auto tests = testsToGroup(entry, filter);

            /* Edge Case: No tests in a file? Don't display it. */
            if (!tests.tests.isEmpty()) {
                displayedTests += tests;
            }
        }

        /* Sort the groups. */
        sort(displayedTests.begin(), displayedTests.end(), [&](const TestGroup& lhs, const TestGroup& rhs) {
            return comparator(lhs.name, rhs.name);
        });

        /* Show everything so there's some basic data available. */
        reporter(displayedTests);

        /* Now, go run the tests. */
        for (auto& group: displayedTests) {
            for (auto& test: group.tests) {
                /* Make clear that we're running the test. */
                test.result = TestResult::RUNNING;
                reporter(displayedTests);

                runSingleTest(test, group);
                reporter(displayedTests);
            }
        }
    }

    void run(TestReporter reporter, TestFilter filter) {
        run(reporter, filter, defaultComparator());
    }
    void run(TestReporter reporter, TestGroupComparator comparator) {
        run(reporter, defaultFilter(), comparator);
    }
    void run(TestReporter reporter) {
        run(reporter, defaultFilter(), defaultComparator());
    }
}
