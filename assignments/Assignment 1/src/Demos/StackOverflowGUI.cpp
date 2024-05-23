#include "../StackOverflow.h"
#include "../GUI/MiniGUI.h"
#include "simpio.h"
using namespace std;
using namespace MiniGUI;

namespace {
    const string kMessage = "Click the button below to trigger a stack overflow. This will crash your program if the debugger is not enabled. If the debugger is enabled, it will jump you to the spot in the code where the stack overflow occurred.";
    const Font kMessageFont(FontFamily::SERIF, FontStyle::NORMAL, 24, "#A50021"); // Madder
    const string kBackgroundColor = "white";

    /* This is an unusual one - it doesn't actually visualize anything, and instead
     * either (1) crashes or (2) aborts with an exception.
     */
    class StackOverflowGUI: public ProblemHandler {
    public:
        StackOverflowGUI(GWindow& window);

        void actionPerformed(GObservable* source) override;

    protected:
        void repaint() override;

    private:
        Temporary<GButton> crashMe;
    };

    StackOverflowGUI::StackOverflowGUI(GWindow& window): ProblemHandler(window) {
        crashMe = make_temporary<GButton>(window, "SOUTH", "Go!");
    }

    void StackOverflowGUI::repaint() {
        clearDisplay(window(), kBackgroundColor);

        auto render = TextRender::construct(kMessage, { 0, 0, window().getCanvasWidth(), window().getCanvasHeight()}, kMessageFont);
        render->alignCenterHorizontally();
        render->alignCenterVertically();
        render->draw(window());
    }

    void StackOverflowGUI::actionPerformed(GObservable* source) {
        if (source == crashMe) {
            triggerStackOverflow(137);
        }
    }
}

GRAPHICS_HANDLER("Stack Overflows", GWindow& window) {
    return make_shared<StackOverflowGUI>(window);
}

CONSOLE_HANDLER("Stack Overflows") {
    if (getYesOrNo("Do you want to trigger a stack overflow? ")) {
        triggerStackOverflow(137);
    }
}
