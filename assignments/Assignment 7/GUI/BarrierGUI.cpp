#include "BarrierGUI.h"
#include "TextUtils.h"
#include "TestDriver.h"
#include "simpio.h"
using namespace std;
using namespace MiniGUI;

namespace {
    const string kTestRunningColor = "white";
    const string kTestFailedColor  = "#800000";

    const Font kTestRunningFont(FontFamily::SERIF, FontStyle::ITALIC, 24, "blue");
    const Font kTestFailedFont (FontFamily::SERIF, FontStyle::ITALIC, 24, "white");

    const string kTestRunningMessage = "Running tests in %s...";
    const string kTestFailedMessage  = "Tests failed in %s. Correct any errors in those tests and select this option again.";

    const double kPadding = 20;

    /* Graphics bounds. */
    GRectangle boundsFor(GWindow& window) {
        return { kPadding, kPadding, window.getCanvasWidth() - 2 * kPadding, window.getCanvasHeight() - 2 * kPadding };
    }

    class BarrierGUI: public ProblemHandler {
    public:
        BarrierGUI(GWindow& window, const set<string>& fails) : ProblemHandler(window) {
            message = format(kTestFailedMessage, conjunctionJoin(fails, "and"));
        }

    protected:
        void repaint() {
            clearDisplay(window(), kTestFailedColor);

            auto render = TextRender::construct(message, boundsFor(window()), kTestFailedFont);
            render->alignCenterVertically();
            render->alignCenterHorizontally();

            render->draw(window());
        }

    private:
        string message;
    };

    /* Given a set of filenames containing tests, returns a set of all the filenames
     * containing tests that failed.
     */
    set<string> failingTestsIn(const set<string>& filenames) {
        Vector<SimpleTest::TestGroup> lastTests;
        SimpleTest::run([&](const Vector<SimpleTest::TestGroup>& tests) {
            lastTests = tests;
        }, [&](const string& testFile, const SimpleTest::Test &) {
            return filenames.count(testFile);
        });

        set<string> result;
        for (const auto& group: lastTests) {
            if (filenames.count(group.name) && group.numPassed != group.numTests) {
                result.insert(group.name);
            }
        }
        return result;
    }
}

function<shared_ptr<ProblemHandler>(GWindow&)>
ifPassedThen(const set<string>& filenames,
             function<shared_ptr<ProblemHandler>(GWindow&)> constructor) {

    return [=](GWindow& window) -> shared_ptr<ProblemHandler> {
        setDemoOptionsEnabled(false);

        clearDisplay(window, kTestRunningColor);
        auto text = TextRender::construct(format(kTestRunningMessage, conjunctionJoin(filenames, "and")),
                                          boundsFor(window),
                                          kTestRunningFont);
        text->alignCenterHorizontally();
        text->alignCenterVertically();
        text->draw(window);
        window.repaint();

        setDemoOptionsEnabled(true);

        /* Run the tests and see if they all passed. */
        auto fails = failingTestsIn(filenames);
        if (fails.empty()) {
            return constructor(window);
        } else {
            return make_shared<BarrierGUI>(window, fails);
        }
    };
}

function<void()>
ifPassedThen(const set<string>& filenames,
             function<void()> callback) {
    return [=] {
        cout << format(kTestRunningMessage, conjunctionJoin(filenames, "and")) << endl;
        auto fails = failingTestsIn(filenames);
        if (fails.empty()) {
            return callback();
        } else {
            cerr << format(kTestFailedMessage, conjunctionJoin(fails, "and")) << endl;
            cerr << "Press ENTER to continue." << endl;
            getLine();
        }
    };
}
