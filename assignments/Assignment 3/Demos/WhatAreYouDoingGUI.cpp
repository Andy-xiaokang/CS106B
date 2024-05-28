#include "GUI/MiniGUI.h"
#include "WhatAreYouDoing.h"
#include "set.h"
#include "ginteractors.h"
#include "tokenscanner.h"
#include "simpio.h"
#include <cctype>
using namespace std;

namespace {
    class WhatAreYouDoingGUI: public ProblemHandler {
    public:
        WhatAreYouDoingGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;

    private:
        Temporary<GColorConsole> mConsole;
        Temporary<GLabel>        mDescription;
        Temporary<GTextField>    mInput;
        Temporary<GButton>       mGo;
        Temporary<GButton>       mClear;
    };

    /* Toggles the case of a string. */
    string toggleCaseIn(string result) {
        for (char& ch: result) {
            if (isalpha(ch)) {
                if (isupper(ch)) ch = tolower(ch);
                else ch = toupper(ch);

            }
        }
        return result;
    }

    Vector<string> sortByEmphasis(const Set<string>& emphases) {
        Vector<string> result;
        for (string option: emphases) {
            result += option;
        }

        sort(result.begin(), result.end(), [](const string& lhs, const string& rhs) {
            /* Reverse the case of the strings before comparing, so that
             * lower case precedes upper-case.
             */
            return toggleCaseIn(lhs) < toggleCaseIn(rhs);
        });

        return result;
    }

    WhatAreYouDoingGUI::WhatAreYouDoingGUI(GWindow& window) : ProblemHandler(window) {
        mConsole     = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
        mDescription = Temporary<GLabel>(new GLabel("Enter sentence: "), window, "SOUTH");
        mInput       = Temporary<GTextField>(new GTextField(""), window, "SOUTH");
        mGo          = Temporary<GButton>(new GButton("Go!"), window, "SOUTH");
        mClear       = Temporary<GButton>(new GButton("Clear"), window, "WEST");
    }

    void WhatAreYouDoingGUI::actionPerformed(GObservable* source) {
        if (source == mGo || source == mInput) {
            string input = mInput->getText();

            mConsole->doWithStyle("#000080", GColorConsole::BOLD_ITALIC, [&] {
                *mConsole << "All emphases of \"" << input << "\":" << '\n';
            });
            for (string option: sortByEmphasis(allEmphasesOf(input))) {
                *mConsole << "  " << option << '\n';
            }
            *mConsole << endl;
        } else if (source == mClear) {
            mConsole->clearDisplay();
            mConsole->flush();
        }
    }
}

Vector<string> tokenize(const string& str) {
    Vector<string> result;

    /* The TokenScanner does most, but not all, of what we want. Specifically, we can
     * either glob together strings that include single quotes or have them broken
     * apart. We'll include quotation marks as part of the string, and then if the
     * string appears to be something in single quotes we'll break it apart.
     */
    TokenScanner scanner(str);
    scanner.addWordCharacters("'");

    while (scanner.hasMoreTokens()) {
        string token = scanner.nextToken();
        if (token.size() > 1 &&
            token.front() == '\'' &&
            token.back()  == '\'') {
            result += "'";
            result += token.substr(1, token.size() - 2);
            result += "'";
        } else {
            result += token;
        }
    }

    return result;
}

GRAPHICS_HANDLER("What Are You Doing?", GWindow& window) {
    return make_shared<WhatAreYouDoingGUI>(window);
}

CONSOLE_HANDLER("What Are You Doing?") {
    do {
        string sentence = getLine("Enter a sentence: ");
        auto result = sortByEmphasis(allEmphasesOf(sentence));

        cout << "There are " << result.size() << " emphases of that sentence. They are: " << endl;
        for (int i = 0; i < result.size(); i++) {
            cout << "  " << (i + 1) << ": " << result[i] << endl;
        }

    } while (getYesOrNo("Would you like to enter another sentence? "));
}
