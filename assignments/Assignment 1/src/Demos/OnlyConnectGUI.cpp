#include "../OnlyConnect.h"
#include "../GUI/MiniGUI.h"
#include "ginteractors.h"
#include "simpio.h"
using namespace std;

namespace {
    class OnlyConnectGUI: public ProblemHandler {
    public:
        OnlyConnectGUI(GWindow& window);
        void actionPerformed(GObservable* source) override;

    private:
        Temporary<GLabel>     inputLabel;
        Temporary<GTextField> inputText;
        Temporary<GButton>    go;

        Temporary<GColorConsole> display;

        /* Button to clear the console. */
        Temporary<GButton> clear;
    };

    OnlyConnectGUI::OnlyConnectGUI(GWindow& window) : ProblemHandler(window) {
        /* Standard buttons. */
        clear = Temporary<GButton>(new GButton("Clear Console"),  window, "WEST");

        /* Raw input. */
        inputLabel = Temporary<GLabel>(new GLabel("Enter text to convert: "), window, "SOUTH");

        inputText = Temporary<GTextField>(new GTextField(), window, "SOUTH");
        go        = Temporary<GButton>(new GButton("Go!"), window, "SOUTH");

        /* Display. */
        display   = Temporary<GColorConsole>(new GColorConsole(), window, "CENTER");
    }

    void processString(GColorConsole& out, const string& text) {
        out << "Input:  " << text << endl;
        out << "Output: ";

        out.doWithStyle("#000080", GColorConsole::BOLD, [&] {
            out << onlyConnectize(text) << endl << endl;
        });
    }

    void OnlyConnectGUI::actionPerformed(GObservable* source) {
        if (source == clear) {
            display->clearDisplay();
            display->flush();
        } else if (source == go || source == inputText) {
            processString(*display, inputText->getText());
            inputText->setText("");
        }
    }
}

GRAPHICS_HANDLER("Only Connect", GWindow& window) {
    return make_shared<OnlyConnectGUI>(window);
}

CONSOLE_HANDLER("Only Connect") {
    do {
        string str = getLine("Enter text to convert: ");
        string result = onlyConnectize(str);
        cout << "Result: " << result << endl;
    } while(getYesOrNo("Convert another string? "));
}
