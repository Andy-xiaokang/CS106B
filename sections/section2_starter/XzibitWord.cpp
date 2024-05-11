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
#include "lexicon.h"
#include <string>
#include "strlib.h"
using namespace std;



string mostXzibitWord(const Lexicon& words) {
    string result;
    int subwordNums = 0;
    for (string word : words) {
        Lexicon subWords;
        for (int i = 0; i < word.size(); i++) {
            for (int j = i; j <= word.size(); j++) {
                string chose = word.substr(i, j-i);
                // if (!words.containsPrefix(chose)) {
                //     break;
                // }
                if (words.contains(chose)) {
                    subWords.add(chose);
                }
            }
        }
        if (subwordNums < subWords.size()) {
            subwordNums = subWords.size();
            result = word;
        }
    }

    return result;
}

/* * * * * Provided Tests Below This Point * * * * */

static Lexicon& sharedLexicon() {
    static Lexicon lex("res/EnglishWords.txt");
    return lex;
}

PROVIDED_TEST("Example from handout") {
    Lexicon& lex = sharedLexicon();
    EXPECT_EQUAL(mostXzibitWord(lex), "foreshadowers");
}
