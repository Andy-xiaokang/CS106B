#include "../GUI/MiniGUI.h"
#include "../Sandpiles.h"
#include "grid.h"
#include "gwindow.h"
#include "hashmap.h"
#include "ginteractors.h"
#include "gtimer.h"
#include <iostream>
#include <cctype>
#include <vector>
using namespace std;
using namespace MiniGUI;

namespace {
    const vector<int> kAdvanceRates = {
        1,
        2,
        4,
        8,
        16,
        32,
        64,
        128,
        256,
        512,
        1024,
        2048,
    };

    const Font kLegendFont(FontFamily::MONOSPACE, FontStyle::BOLD, 14, "#343434"); // Jet
    const double kFontHeight = 24;


    /* Time to pause between frames. */
    const double kPauseTime = 100;

    /* World padding. */
    const double kPadding = 30;

    /* Maximum cell size. */
    const double kMaxCellSize = 10;

    /* World sizes. */
    const vector<string> kSizeClasses = {
        "Tiny",
        "Small",
        "Medium",
        "Large",
        "Huge"
    };

    /* Size classes. */
    const HashMap<string, pair<int, int>> kSizes = {
        { "Tiny",   {  15,  15 } },
        { "Small",  {  31,  31 } },
        { "Medium", {  63,  63 } },
        { "Large",  { 127, 127 } },
        { "Huge",   { 255, 255 } },
    };

    /* Background color and cell colors. */
    const string kBackgroundColor = "#727472"; // Nickel
    const Vector<string> kColors = {
        "#000000",
        "#303080",
        "#8080A0",
        "#CFCFFF"
    };

    struct Geometry {
        double baseX, baseY;
        double cellSize;
    };

    Geometry geometryFor(const Grid<int>& world, GWindow& window) {
        /* Computing the scaling factors needed to scale to the window width and window height. */
        double width  = window.getCanvasWidth()  - 2 * kPadding;
        double height = window.getCanvasHeight() - 2 * kPadding;

        double scaleX = width  / world.numCols();
        double scaleY = height / world.numRows();
        double scale = min({ scaleX, scaleY, kMaxCellSize });

        /* Compute base x and y based on the scale. */
        double baseX = kPadding + (width  - world.numCols() * scale) / 2.0;
        double baseY = kPadding + (height - world.numRows() * scale) / 2.0;

        return {
            baseX, baseY,
            scale
        };
    }

    void drawWorld(const Grid<int>& world, GWindow& window) {
        /* Clear the entire display. */
        window.setColor(kBackgroundColor);
        window.fillRect(0, 0, window.getCanvasWidth(), window.getCanvasHeight());

        auto g = geometryFor(world, window);

        /* Optimization: Don't draw empty cells. */
        window.setColor(kColors[0]);
        window.fillRect(g.baseX, g.baseY, world.numCols() * g.cellSize, world.numRows() * g.cellSize);

        GRect toDraw(0, 0, g.cellSize, g.cellSize);
        toDraw.setFilled(true);

        for (int row = 0; row < world.numRows(); row++) {
            for (int col = 0; col < world.numCols(); col++) {
                /* Optimization: don't draw empty cells. */
                if (world[row][col] == 0) continue;

                double x = col * g.cellSize + g.baseX;
                double y = row * g.cellSize + g.baseY;

                toDraw.setLocation(x, y);
                toDraw.setFillColor(kColors[world[row][col]]);
                toDraw.setColor(kColors[world[row][col]]);
                window.draw(toDraw);
            }
        }
    }

    /* Inserts commas into a numeric string. */
    string addCommasTo(int val) {
        string asStr = to_string(val);
        const int length = int(asStr.length());

        string result;
        for (int i = 0; i < length; i++) {
            /* Run backwards through the source string so determining where commas go
             * becomes a lot easier.
             */
            result = asStr[length - 1 - i] + result;

            /* Put commas in provided we've already added three characters, but aren't
             * about to use all the digits up.
             */
            if (i % 3 == 2 && i < length - 1) {
                result = ',' + result;
            }
        }
        return result;
    }

    /* Renders a legend describing how many grains of sand have been dropped. */
    void drawLegend(int grainsDropped, GWindow& window) {
        GRectangle bounds {
            0, window.getCanvasHeight() - kFontHeight,
            window.getCanvasWidth(),
            kFontHeight
        };

        string text = addCommasTo(grainsDropped) + (grainsDropped == 1? " sand grain dropped." : " sand grains dropped.");

        auto render = TextRender::construct(text, bounds, kLegendFont, LineBreak::NO_BREAK_SPACES);
        render->alignCenterVertically();
        render->draw(window);
    }

    /* Problem handler for visualizing a growing sandpile. */
    class SandpileGUI: public ProblemHandler {
    public:
        SandpileGUI(GWindow& window);
        void actionPerformed(GObservable* source) override;
        void timerFired() override;

    protected:
        void repaint() override;

    private:
        /* Current state of the world. */
        Grid<int> world;

        /* Timer to drive things. */
        unique_ptr<GTimer> timer;

        /* Buttons to control the world size. */
        Temporary<GLabel> sizeLabel;
        vector<Temporary<GButton>> sizeButtons;

        /* Slider to control speed. */
        Temporary<GLabel> speedLabel;
        Temporary<GSlider> speedControl;

        /* How many grains of sand we've dropped. */
        int grainsDropped = 0;
    };

    SandpileGUI::SandpileGUI(GWindow& window) : ProblemHandler(window) {
        world.resize(kSizes["Medium"].first, kSizes["Medium"].second);

        /* Size buttons. */
        sizeLabel = Temporary<GLabel>(new GLabel("World Size: "), window, "SOUTH");
        for (const auto& size: kSizeClasses) {
            sizeButtons.emplace_back(new GButton(size), window, "SOUTH");
        }

        /* Speed control. */
        speedLabel = Temporary<GLabel>(new GLabel("Drop rate: "), window, "SOUTH");
        GSlider* slider = new GSlider(0, kAdvanceRates.size() - 1, 0);
        slider->setPaintTicks(true);
        speedControl = Temporary<GSlider>(slider, window, "SOUTH");

        timer.reset(new GTimer(kPauseTime));
        timer->start();
    }

    void SandpileGUI::repaint() {
        drawWorld(world, window());
        drawLegend(grainsDropped, window());
    }

    void SandpileGUI::actionPerformed(GObservable* source) {
        /* If the source is one of our buttons, change the world size. */
        auto itr = find(sizeButtons.begin(), sizeButtons.end(), source);
        if (itr != sizeButtons.end()) {
            auto size = kSizes[(*itr)->getText()];
            world.resize(size.first, size.second);
            world.fill(0);
            grainsDropped = 0;
            requestRepaint();
        }
    }

    void SandpileGUI::timerFired() {
        int numSteps = kAdvanceRates[speedControl->getValue()];
        for (int i = 0; i < numSteps; i++) {
            dropSandOn(world, world.numRows() / 2, world.numCols() / 2);
            ++grainsDropped;
        }
        requestRepaint();
    }
}

GRAPHICS_HANDLER("Sandpiles", GWindow& window) {
    return make_shared<SandpileGUI>(window);
}

namespace {
    void drawWorld(const Grid<int>& world) {
        cout << "Current configuration:" << endl;
        for (int row = 0; row < world.numRows(); row++) {
            for (int col = 0; col < world.numCols(); col++) {
                cout << world[row][col] << " ";
            }
            cout << endl;
        }
    }
}

CONSOLE_HANDLER("Sandpiles") {
    Grid<int> world(5, 5);
    drawWorld(world);
    while (getYesOrNo("Drop a grain of sand in the middle? ")) {
        dropSandOn(world, world.numRows() / 2, world.numCols() / 2);
        drawWorld(world);
    }
}
