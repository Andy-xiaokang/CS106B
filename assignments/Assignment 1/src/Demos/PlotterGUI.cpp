#include "../Plotter.h"
#include "../GUI/MiniGUI.h"
#include "gobjects.h"
#include "error.h"
#include "filelib.h"
#include "ginteractors.h"
#include <memory>
#include <vector>
using namespace std;

namespace {
    /* Padding from window border to content area. */
    const double kPadding = 20;

    /* File suffix for plotter files. */
    const string kFileSuffix = ".plot";

    /* "Not selected" option. */
    const string kNotSelected = "-";

    /* Text for load button. */
    const string kLoadButtonText = "Plot!";

    /* Window background color - used for everything but the canvas. */
    const string kWindowColor = "#A4DDED"; // Non-Photo Blue: dark enough to clearly make a
                                           // border, not so dark that lines on the border
                                           // won't be seen

    /* Canvas color. */
    const string kCanvasColor = "#FFFFFF"; // Pure white, for simplicity

    class PlotterGUI: public ProblemHandler {
    public:
        PlotterGUI(GWindow& window);
        ~PlotterGUI();

        void addLine(double x0, double y0, double x1, double y1, PenStyle style);
        void actionPerformed(GObservable* obj) override;
        void windowResized() override;

    protected:
        void repaint() override;

    private:
        /* Geometry. */
        double baseX, baseY, scale;

        /* Lines to plot. */
        struct Line {
            double x0, y0, x1, y1;
            PenStyle style;
        };

        /* Raw lines in abstract space. */
        vector<Line> rawLines;

        /* Lines to draw on the screen. This list may be shorter than rawLines
         * in the event that a line has been added that hasn't been drawn yet,
         * or if the window is resized. Make sure to sync the size of rawLines
         * and the size of displayLines before drawing!
         */
        vector<unique_ptr<GLine>> displayLines;

        /* Two-row grid of items. The layout looks like this
         *
         *                     Dropdown   Load
         *    ------------------- status line ------------------------
         */
        Temporary<GContainer> container;

        /* Load file. */
        GComboBox*  fileChooser;
        GButton*    loadButton;

        /* Status reporting. */
        GLabel*     statusLine;

        /* Initialization helpers. */
        void calculateGeometry();
        void setUpChrome();

        /* Resets graphics state. */
        void clearGraphics();

        /* Syncs the displayLines list with rawLines. */
        void createGraphicsLines();
    };

    /* Returns all plotter files found in the example directory. */
    vector<string> allPlotFiles() {
        vector<string> result;
        for (const auto& file: listDirectory("res/")) {
            if (endsWith(file, kFileSuffix)) {
                result.push_back(file);
            }
        }
        return result;
    }

    /* Ewww - global variables! */
    PlotterGUI* theGUI = nullptr;

    /* Constructor sets up graphics and hooks us in as the One True GUI. */
    PlotterGUI::PlotterGUI(GWindow& window) : ProblemHandler(window) {
        if (theGUI) error("Why are there two copies of us?");
        theGUI = this;

        setUpChrome();
        calculateGeometry();
    }

    PlotterGUI::~PlotterGUI() {
        theGUI = nullptr;
    }

    /* Hook from the global graphics system to drawing a line. */
    void PlotterGUI::addLine(double x0, double y0, double x1, double y1, PenStyle style) {
        rawLines.push_back({x0, y0, x1, y1, style});
        requestRepaint();
    }

    /* Calculates window geometry information - scale, base X, base Y, etc. */
    void PlotterGUI::calculateGeometry() {
        /* Computing the scaling factors needed to scale to the window width and window height. */
        double width  = window().getCanvasWidth()  - 2 * kPadding;
        double height = window().getCanvasHeight() - 2 * kPadding;

        scale = min(width, height) / 2.0;

        /* Compute base x and y based on the scale. */
        baseX = kPadding + width / 2.0;
        baseY = kPadding + height / 2.0;
    }

    /* Sets up window controls ("chrome"). */
    void PlotterGUI::setUpChrome() {
        auto* rawContainer = new GContainer(GContainer::LAYOUT_GRID);

        fileChooser = new GComboBox();
        fileChooser->addItem(kNotSelected); // Initially, nothing shows
        for (const string& file: allPlotFiles()) {
            fileChooser->addItem(file);
        }
        fileChooser->setEditable(false);

        loadButton  = new GButton(kLoadButtonText);
        statusLine  = new GLabel("");

        rawContainer->addToGrid(fileChooser, 0, 0);
        rawContainer->addToGrid(loadButton,  0, 1);
        rawContainer->addToGrid(statusLine,  1, 0, 1, 2);

        container = Temporary<GContainer>(rawContainer, window(), "SOUTH");
    }

    void PlotterGUI::repaint() {
        /* We have to draw the border on top of the lines in case the plotter
         * goes out of bounds!
         */
        clearDisplay(window(), kCanvasColor);
        createGraphicsLines();

        for (const auto& line: displayLines) {
            window().draw(*line);
        }

        /* Draw above, to the left of, to the right of, and below the canvas. */
        window().setColor(kWindowColor);
        window().fillRect(0, 0, baseX - scale, window().getHeight()); // Left
        window().fillRect(baseX + scale, 0, window().getWidth() - baseX - scale, window().getHeight()); // Right
        window().fillRect(0, 0, window().getWidth(), baseY - scale);
        window().fillRect(0, baseY + scale, window().getWidth(), window().getHeight() - baseY - scale);
    }

    void PlotterGUI::createGraphicsLines() {
        /* Add new lines to the end of the list. */
        for (size_t i = displayLines.size(); i < rawLines.size(); i++) {
            Line curr = rawLines[i];

            /* TODO: Once C++14 support is everywhere, use make_unique. */
            unique_ptr<GLine> line(new GLine(curr.x0 * scale + baseX, -curr.y0 * scale + baseY,
                                             curr.x1 * scale + baseX, -curr.y1 * scale + baseY));
            line->setColor(curr.style.color);
            line->setLineWidth(curr.style.width);
            displayLines.push_back(move(line));
        }
    }

    void PlotterGUI::actionPerformed(GObservable* obj) {
        /* Was it the load button? If so, load things. */
        if (obj == loadButton) {
            string toLoad = fileChooser->getSelectedItem();
            if (toLoad != kNotSelected) {
                clearGraphics();

                ifstream input("res/" + toLoad);
                runPlotterScript(input);
            }
        }
    }

    void PlotterGUI::clearGraphics() {
        displayLines.clear();
        rawLines.clear();
        requestRepaint();
    }

    void PlotterGUI::windowResized() {
        /* Wipe all previously-displayed lines; they have the wrong position. */
        displayLines.clear();

        calculateGeometry();
        requestRepaint();
    }
}

/* Entry point for student calls to drawLine. */
void drawLine(double x0, double y0, double x1, double y1, PenStyle style) {
    if (MiniGUI::Config::isConsoleMode()) {
        cout << "Line drawn from (" << x0 << ", " << y0 << ") to (" << x1 << ", " << y1 << "), width " << style.width << ", color " << style.color << endl;
    } else {
        if (theGUI) {
            theGUI->addLine(x0, y0, x1, y1, style);
        }
    }
}

GRAPHICS_HANDLER("Plotter", GWindow& window) {
    return make_shared<PlotterGUI>(window);
}

CONSOLE_HANDLER("Plotter") {
    cout << "The console version of Plotter will display the "
            "coordinates and styles of all lines drawn with the drawLine command." << endl;
    do {
        auto options = allPlotFiles();
        int choice = makeSelectionFrom("Choose a file to plot: ", options);

        ifstream input("res/" + options[choice], ios::binary);
        if (!input) {
            cerr << "Error: Can't open file " << choice << endl;
        } else {
            cout << "Running your script..." << endl;
            runPlotterScript(input);
            cout << "Done." << endl;
        }
    } while (getYesOrNo("Draw another plot? "));
}
