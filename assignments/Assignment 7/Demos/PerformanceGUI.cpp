#include "ChainedHashTable.h"
#include "LinearProbingHashTable.h"
#include "RobinHoodHashTable.h"
#include "MyOptionalHashTable.h"
#include "GUI/MiniGUI.h"
#include "Timer.h"
#include "HashFunction.h"
#include "ginteractors.h"
#include "gthread.h"
#include <tuple>
#include <iomanip>
#include <fstream>
#include <iterator>
#include <vector>
#include <random>
using namespace std;

namespace {
    /* Timing result struct. */
    struct TimingResult {
        double insertSuccessTime;
        double insertFailTime;
        double lookupSuccessTime;
        double lookupFailTime;
        double removeSuccessTime;
        double removeFailTime;
    };

    /* Hash traits. */
    template <typename Table> struct HashTraits;

    /* X Macros to import definition. */
    #define HASH_TABLE(type, desc)                \
        template <> struct HashTraits<type> {     \
            static string name() { return desc; } \
        };
    #include "TimeTestConfig.h"
    #undef HASH_TABLE

    template <typename...> struct TypeList {};

    /* Metafunction to prepend to a type list. */
    template <typename Type, typename List> struct Prepend;
    template <typename Type, typename... Types> struct Prepend<Type, TypeList<Types...>> {
        using type = TypeList<Type, Types...>;
    };

    /* Metafunction to remove the last item from a typelist. */
    template <typename...> struct RemoveLast;
    template <typename T> struct RemoveLast<T> {
        using type = TypeList<>;
    };
    template <typename First, typename Second, typename... Rest> struct RemoveLast<TypeList<First, Second, Rest...>> {
        using type = typename Prepend<First, typename RemoveLast<TypeList<Second, Rest...>>::type>::type;
    };

    /* List of all the available tables. */
    using AllHashTables = typename RemoveLast<TypeList<
        #define HASH_TABLE(type, desc) type,
        #include "TimeTestConfig.h"
        #undef HASH_TABLE
        void
    >>::type;

    /* Applies something to all table types. */
    template <typename TestCase> void applyTo(TestCase&, TypeList<>) {
      /* Base case: There are no types to apply the testing function to. */
    }

    /* Recursive case: There's at least one type to test. */
    template <typename TestCase, typename HashTable, typename... HashTables>
    void applyTo(TestCase& testCase, TypeList<HashTable, HashTables...>) {
      /* Invoke the testing function on this type */
      testCase.template test<HashTable>();

      /* Recursively invoke applyTo, passing in the tail end of the list of tree
       * types.
       */
      applyTo<TestCase>(testCase, TypeList<HashTables...>());
    }

    class PerformanceGUI: public ProblemHandler {
    public:
        PerformanceGUI(GWindow& window);

        void settingUp() override;

    private:
        Temporary<GBrowserPane> mContent;

        void runAllTests();
        void displayResults(const vector<vector<TimingResult>>& results);
    };

    PerformanceGUI::PerformanceGUI(GWindow& window) : ProblemHandler(window) {
        mContent = make_temporary<GBrowserPane>(window, "CENTER");

        setDemoOptionsEnabled(false);
    }

    void PerformanceGUI::settingUp() {
        runAllTests();
        setDemoOptionsEnabled(true);
    }

    vector<string> loadEnglishWords() {
        ifstream input("res/EnglishWords.txt");
        return { istream_iterator<string>(input), istream_iterator<string>() };
    }

    const int kNumIterations = 10;

    template <typename Table> TimingResult timeTest(double loadFactor) {
        Timing::Timer constructionTimer;
        Timing::Timer insertSuccessTimer;
        Timing::Timer insertFailTimer;
        Timing::Timer lookupSuccessTimer;
        Timing::Timer lookupFailTimer;
        Timing::Timer removeSuccessTimer;
        Timing::Timer removeFailTimer;

        /* Load all English words. */
        auto english = loadEnglishWords();

        /* Determine how many slots we'll need. */
        int numSlots = english.size() / loadFactor;

        /* Random source for shuffling. */
        mt19937 generator(randomInteger(0, INT_MAX));

        for (int iteration = 0; iteration < kNumIterations; iteration++) {
            constructionTimer.start();

            /* This line of code constructs the hash table. If you have a custom hash function
             * you'd like to use here instead of our standard selection of random hash functions.
             * switch this line to read
             *
             *    Table table(HashFunction<string>::wrap(numSlots, your-hash-function));
             */
            Table table(Hash::random(numSlots));
            constructionTimer.stop();

            for (string& word: english) {
                word = toLowerCase(word);
            }

            for (const auto& word: english) {
                insertSuccessTimer.start();
                if (!table.insert(word)) {
                    error("Couldn't insert \"" + word + "\" into the hash table.");
                }
                insertSuccessTimer.stop();

                insertFailTimer.start();
                if (table.insert(word)) {
                    error("Inserted duplicate word \"" + word + "\" into the hash table.");
                }
                insertFailTimer.stop();
            }

            /* Query for all items in the table, in random order. */
            shuffle(english.begin(), english.end(), generator);

            for (const auto& word: english) {
                lookupSuccessTimer.start();
                if (!table.contains(word)) {
                    error("Couldn't locate word \"" + word + "\" in the hash table.");
                }
                lookupSuccessTimer.stop();
            }

            /* Query for things that aren't there. */
            for (string& word: english) {
                word = toUpperCase(word);
            }

            for (const auto& word: english) {
                lookupFailTimer.start();
                if (table.contains(word)) {
                    error("Found word \"" + word + "\", which is not in the hash table.");
                }
                lookupFailTimer.stop();
            }

            /* Remove all the words that aren't there. */
            shuffle(english.begin(), english.end(), generator);

            for (const auto& word: english) {
                removeFailTimer.start();
                if (table.remove(word)) {
                    error("Removed word \"" + word + "\", which is not in the hash table.");
                }
                removeFailTimer.stop();

                string toRemove = toLowerCase(word);
                removeSuccessTimer.start();
                if (!table.remove(toRemove)) {
                    error("Couldn't remove word \"" + word + "\", which is in the hash table.");
                }
                removeSuccessTimer.stop();
            }
        }


        return {
            insertSuccessTimer.elapsed()   / (english.size() * kNumIterations),
            insertFailTimer.elapsed()      / (english.size() * kNumIterations),
            lookupSuccessTimer.elapsed()   / (english.size() * kNumIterations),
            lookupFailTimer.elapsed()      / (english.size() * kNumIterations),
            removeSuccessTimer.elapsed()   / (english.size() * kNumIterations),
            removeFailTimer.elapsed()      / (english.size() * kNumIterations),
        };
    }

    /* All load factors. */
    const vector<double> kLoadFactors = {
        0.50,
        0.60,
        0.70,
        0.80,
        0.85,
        0.90,
        0.93,
        0.96,
        0.97,
    };

    /* Test evaluator - runs the test and stashes away the result. */
    struct Evaluator {
        const double loadFactor;
        vector<TimingResult> results;

        Evaluator(double load) : loadFactor(load) {};

        template <typename Table> void test() {
            results.push_back(timeTest<Table>(loadFactor));
        }
    };

    /* Generic printf. */
    string format(const string& pattern) {
        /* If there's a replacement site, something is wrong. */
        if (pattern.find("%s") != string::npos) {
            error("Unmatched pattern string?");
        }
        return pattern;
    }

    string niceToString(double value) {
        ostringstream converter;
        converter << fixed << setprecision(2) << value;
        return converter.str();
    }

    template <typename First, typename... Args>
    string format(const string& pattern, First&& first, Args&&... args) {
        size_t toReplace = pattern.find("%s");
        if (toReplace == string::npos) {
            error("No pattern to replace?");
        }

        return    pattern.substr(0, toReplace)
                + to_string(std::forward<First>(first))
                + format(pattern.substr(toReplace + 2), args...);
    }

    const string kHTMLHeader =
            R"(<html>
                <head>
                </head>
                <body style="color:black;background-color:white;font-size:13pt;">)";

    const string kHTMLFooter =
            R"(</body></html>)";

    const string kTableIntro =
            R"(<table cellpadding="3" cellspacing="0" align="center">)";

    const string kTableOutro =
            R"(</table>)";

    /* Creates the header used by the table. */
    struct HeaderBuilder {
        ostream& out;
        template <typename Table> void test() {
            out << "<th>" << HashTraits<Table>::name() << "</th>";
        }
    };

    void makeTableHeader(ostream& out) {
        /* Two blank entries for things that don't need headers. */
        out << "<tr><th></th><th></th>";

        /* Generate headers for all the relevant types. */
        HeaderBuilder builder{out};
        applyTo(builder, AllHashTables());

        out << "</tr>";
    }

    /* Info about how to access data from each row. */
    struct RowInfo {
        string title;
        double TimingResult::* field;
    };
    const vector<RowInfo> kRows = {
        { "Insert (success)", &TimingResult::insertSuccessTime },
        { "Insert (failure)", &TimingResult::insertFailTime    },
        { "Lookup (success)", &TimingResult::lookupSuccessTime },
        { "Lookup (failure)", &TimingResult::lookupFailTime    },
        { "Remove (success)", &TimingResult::removeSuccessTime },
        { "Remove (failure)", &TimingResult::removeFailTime    },
    };

    /* Value to display in an empty cell. */
    const string kEmptyValue = "<i>waiting</i>";

    string styleFor(int row) {
        return format("background-color:%s;border: 3px solid black; border-collapse:collapse;",
                      row % 2 == 0? "#ffff80" : "white");
    }

    void makeTableRows(ostream& out,
                       const vector<vector<TimingResult>>& results,
                       size_t numCols) {
        /* Loop over all the load factors, which make up the rows of the table. */
        for (size_t row = 0; row < kLoadFactors.size(); row++) {
            out << format("<tr style=\"%s\">", styleFor(row));
            out << format("<td rowspan=\"%s\">&alpha; = ", kRows.size()) << kLoadFactors[row] << "</td>";

            for (size_t section = 0; section < kRows.size(); section++) {
                out << format("<td>%s</td>", kRows[section].title);

                for (size_t col = 0; col < numCols; col++) {
                    string value = (row < results.size()? niceToString(results[row][col].*kRows[section].field) + "ns" : kEmptyValue);
                    out << format("<td>%s</td>", value);
                }

                /* Weird edge case: since we opened a row up front for the header, we need to close
                 * this row and then reopen one if there's another row coming.
                 */
                out << "</tr>";
                if (section + 1 != kRows.size()) out << format("<tr style=\"%s\">", styleFor(row));;
            }
        }
    }

    template <typename... Args>
    size_t sizeOf(TypeList<Args...>) {
        return sizeof...(Args);
    }

    void PerformanceGUI::displayResults(const vector<vector<TimingResult>>& results) {
        ostringstream builder;
        builder << kHTMLHeader << kTableIntro;

        makeTableHeader(builder);
        makeTableRows(builder, results, sizeOf(AllHashTables()));

        builder << kTableOutro << kHTMLFooter;

        /* Update the HTML display synchronously so we don't have other threads fighting
         * with us.
         */
        GThread::runOnQtGuiThread([&] {
            mContent->setText(builder.str());
        });
    }

    void PerformanceGUI::runAllTests() {
        /* Grid of all test results. */
        vector<vector<TimingResult>> results;

        displayResults(results);
        for (double load: kLoadFactors) {
            Evaluator evaluator(load);
            applyTo(evaluator, AllHashTables());
            results.push_back(evaluator.results);
            displayResults(results);
        }
    }
}

GRAPHICS_HANDLER("Performance Analysis", GWindow& window) {
    return make_shared<PerformanceGUI>(window);
}

namespace {
    void printResults(const vector<TimingResult>& results, TypeList<>, size_t index) {
        if (index != results.size()) {
            error("Internal error: Size mismatch.");
        }
    }

    template <typename First, typename... Rest>
    void printResults(const vector<TimingResult>& results, TypeList<First, Rest...>, size_t index) {
        if (index >= results.size()) {
            error("Internal error: Size mismatch.");
        }

        cout << HashTraits<First>::name() << endl;
        for (auto row: kRows) {
            cout << "  " << row.title << ": " << results[index].*row.field << endl;
        }
        cout << endl;
        printResults(results, TypeList<Rest...>(), index + 1);
    }

    void displayResults(const vector<TimingResult>& results) {
        printResults(results, AllHashTables(), 0);
    }
}

CONSOLE_HANDLER("Performance Analysis") {
    for (double load: kLoadFactors) {
        cout << "Evaluating on load factor alpha = " << load << "..." << endl;
        Evaluator evaluator(load);
        applyTo(evaluator, AllHashTables());
        displayResults(evaluator.results);
    }
}
