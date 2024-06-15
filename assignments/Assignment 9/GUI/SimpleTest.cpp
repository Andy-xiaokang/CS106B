#include "SimpleTest.h"
#include <string>
using namespace std;

namespace SimpleTest {
    namespace Internal {
        /* Why this type? Because we want to be able to
         *
         * 1. find all the tests in a certain group, and
         * 2. have them be sorted by line number.
         */
        map<TestKey, multimap<int, TestCase>>& rawTests() {
            static map<TestKey, multimap<int, TestCase>> result;
            return result;
        }

        void doFail(const string& message, size_t line) {
            throw TestFailedException(message, line);
        }
        void doExpect(bool condition, const string &expression, size_t line) {
            if (!condition) {
                doFail(expression, line);
            }
        }

        void showError(const std::string& message, size_t line) {
            doFail(message, line);
        }

        /* TestFailedException implementation. */
        TestFailedException::TestFailedException(const string& message, std::size_t line)
            : logic_error("Line " + to_string(line) + ": " + string(message)) {

        }

        /* TestCaseAdder implementation. */
        TestCaseAdder::TestCaseAdder(const TestKey& key, int line, const string& name, TestType type, std::function<void()> test) {
            rawTests()[key].insert(make_pair(line, TestCase{ name, type, line, test }));
        }
    }
}
