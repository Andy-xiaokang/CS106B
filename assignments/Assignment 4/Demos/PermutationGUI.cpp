#include "GUI/MiniGUI.h"
#include "Permutations.h"
#include "set.h"
#include "ginteractors.h"
#include "tokenscanner.h"
#include "simpio.h"
#include <cctype>
using namespace std;

namespace {
    class PermutationGUI: public ProblemHandler {
    public:
        PermutationGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;

    private:
        Temporary<GColorConsole> mConsole;
        Temporary<GLabel>        mDescription;
        Temporary<GTextField>    mInput;
        Temporary<GButton>       mGo;
        Temporary<GButton>       mClear;

        void showPermutationsOf(const string& str);
    };

    PermutationGUI::PermutationGUI(GWindow& window) : ProblemHandler(window) {
        mConsole     = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
        mDescription = Temporary<GLabel>(new GLabel("Enter string: "), window, "SOUTH");
        mInput       = Temporary<GTextField>(new GTextField(""), window, "SOUTH");
        mGo          = Temporary<GButton>(new GButton("Go!"), window, "SOUTH");
        mClear       = Temporary<GButton>(new GButton("Clear"), window, "WEST");
    }

    void PermutationGUI::actionPerformed(GObservable* source) {
        if (source == mGo || source == mInput) {
            setDemoOptionsEnabled(false);
            mInput->setEnabled(false);
            mGo->setEnabled(false);

            mConsole->doWithStyle("#808080", GColorConsole::ITALIC, [&] {
                *mConsole << "Computing Permutations... " << flush;
            });
            showPermutationsOf(mInput->getText());

            mGo->setEnabled(true);
            mInput->setEnabled(true);
            setDemoOptionsEnabled(true);

        } else if (source == mClear) {
            mConsole->clearDisplay();
            mConsole->flush();
        }
    }

    void PermutationGUI::showPermutationsOf(const string& str) {
        auto permutations = permutationsOf(str);
        mConsole->doWithStyle("#000080", GColorConsole::BOLD_ITALIC, [&] {
            *mConsole << "Done!" << endl;
        });
        mConsole->doWithStyle("#000000", GColorConsole::NORMAL, [&] {
            for (string permutation: permutations) {
                *mConsole << "  \"" << permutation << '"' << endl;
            }
        });
        *mConsole << endl;
    }
}

GRAPHICS_HANDLER("Permutations", GWindow& window) {
    return make_shared<PermutationGUI>(window);
}

CONSOLE_HANDLER("Permutations") {
    do {
        string input = getLine("Enter a string: ");
        cout << "Computing Permutations..." << endl;
        auto permutations = permutationsOf(input);

        int number = 0;
        for (string permutation: permutations) {
            cout << "Permutation #" << (++number) << ": \"" << permutation << "\"" << endl;
        }
    } while (getYesOrNo("Do you want to see permutations of another string? "));
}
