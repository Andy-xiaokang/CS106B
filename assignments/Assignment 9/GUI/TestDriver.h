#pragma once

#include "vector.h"
#include <string>
#include <cstdint>
#include <functional>

namespace SimpleTest {
    /* Type describing what sort of test a test is. */
    enum class TestType {
        STUDENT,
        PROVIDED,
        AUTOGRADER,
        MANUAL
    };

    /* Type representing how a test turned out. */
    enum class TestResult {
        WAITING,   // Test not yet run
        RUNNING,   // Test is currently running
        PASS,      // Test passed
        FAIL,      // Test failed due to EXPECT, EXPECT_ERROR, etc. failing
        LEAK,      // Test passed, but had a memory leak
        EXCEPTION  // Test failed due to an exception
    };

    /* Type representing a single test. */
    struct Test {
        /* Basic information. */
        std::string name;
        TestType type;
        int lineNumber;

        /* Test results. */
        TestResult result;
        std::string detailMessage;

        /* For internal use only. */
        std::function<void()> callback;
    };


    /* Type representing a group of tests. */
    struct TestGroup {
        std::string name;

        Vector<Test> tests;
        size_t numTests;
        size_t numPassed;
    };

    /* Type representing a display interface that can receive and report
     * information about tests.
     */
    using TestReporter = std::function<void(const Vector<TestGroup> &)>;

    /* Type representing a filter on which tests should be run. The input is the
     * test's group name and the test itself, and the output is true if the test
     * should be run and false otherwise.
     */
    using TestFilter = std::function<bool(const std::string&, const Test&)>;

    /* Type representing a comparator used to determine the order in which groups
     * should be displayed.
     */
    using TestGroupComparator = std::function<bool(const std::string&, const std::string&)>;

    /* Runs all tests that match the given filter, reporting groups in the order given by
     * the comparator.
     *
     * If no filter is specified, all tests are run.
     *
     * If no comparator is specified, test groups are ordered alphabetically.
     */
    void run(TestReporter, TestFilter, TestGroupComparator);
    void run(TestReporter, TestFilter);
    void run(TestReporter, TestGroupComparator);
    void run(TestReporter);
}
