/*
 * CS106B Section Handout Test Harness: Section 2
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified for Qt and Ed by Neel
 * Kishnani for Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include "testing/SimpleTest.h"
#include "set.h"
#include "filelib.h"
using namespace std;

/*
 * See Stanford library's promptUserForFile function to get
 * a file into a stream format:
 * https://web.stanford.edu/dept/cs_edu/resources/cslib_docs/filelib.html#Function:promptUserForFile
 */
Set<string> wordsIn(istream& input) {
    Set<string> result;
    string word;
    while (input >> word) {
        result.add(word);
    }
    return result;
}

int main() {
    ifstream input1, input2;
    promptUserForFile(input1, "Enter filename1: ");
    promptUserForFile(input2, "Enter filename2: ");
    Set<string> wordSet1 = wordsIn(input1);
    Set<string> wordSet2 = wordsIn(input2);
    double intersectionSize = (wordSet1 * wordSet2).size();
    double unionSize = (wordSet1 + wordSet2).size();
    cout << "Jaccard similarityL: " << intersectionSize / unionSize << endl;
    return 0;
}

