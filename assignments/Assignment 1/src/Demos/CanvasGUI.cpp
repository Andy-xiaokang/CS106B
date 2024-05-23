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

    /* Window background color - used for everything but the canvas. */
    const string kWindowColor = "#A4DDED"; // Non-Photo Blue: dark enough to clearly make a
                                           // border, not so dark that lines on the border
                                           // won't be seen

    /* Canvas color. */
    const string kCanvasColor = "#FFFFFF"; // Pure white, for simplicity

    class PlotterGUI: public ProblemHandler {
    public:
        PlotterGUI(GWindow& window);

        void actionPerformed(GObservable* obj) override;
        void mousePressed(double x, double y) override;
        void mouseDragged(double x, double y) override;

    protected:
        void repaint() override;

    private:
        void addLine(double x, double y, double width, double height, PenStyle style);

        /* Geometry. */
        double baseX, baseY, scale;

        double lastX, lastY; // In real coordinates

        double mouseToX(double mouseX) const;
        double mouseToY(double mouseY) const;

        /* Lines to plot. */
        vector<unique_ptr<GLine>> lines;
        ostringstream commands;

        Temporary<GTextField> mFilename;
        Temporary<GButton> mSave;
        Temporary<GTextField> mColor;
        Temporary<GTextField> mWidth;

        /* Initialization helpers. */
        void calculateGeometry();
        void setUpChrome();
    };

    /* Constructor sets up graphics and hooks us in as the One True GUI. */
    PlotterGUI::PlotterGUI(GWindow& window) : ProblemHandler(window) {
        setUpChrome();
        calculateGeometry();
    }

    /* Hook from the global graphics system to drawing a line. */
    void PlotterGUI::addLine(double x0, double y0, double x1, double y1, PenStyle style) {
        /* TODO: Once C++14 support is everywhere, use make_unique. */
        unique_ptr<GLine> line(new GLine(x0 * scale + baseX, -y0 * scale + baseY,
                                         x1 * scale + baseX, -y1 * scale + baseY));
        line->setColor(style.color);
        line->setLineWidth(style.width);
        lines.push_back(move(line));

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
        mFilename = Temporary<GTextField>(new GTextField("(filename)"), window(), "SOUTH");
        mSave     = Temporary<GButton>(new GButton("Save"), window(), "SOUTH");
        mColor    = Temporary<GTextField>(new GTextField("black"), window(), "SOUTH");
        mWidth    = Temporary<GTextField>(new GTextField("1"), window(), "SOUTH");
    }

    void PlotterGUI::repaint() {
        /* We have to draw the border on top of the lines in case the plotter
         * goes out of bounds!
         */
        clearDisplay(window(), kCanvasColor);

        for (const auto& line: lines) {
            window().draw(*line);
        }

        /* Draw above, to the left of, to the right of, and below the canvas. */
        window().setColor(kWindowColor);
        window().fillRect(0, 0, baseX - scale, window().getHeight()); // Left
        window().fillRect(baseX + scale, 0, window().getWidth() - baseX - scale, window().getHeight()); // Right
        window().fillRect(0, 0, window().getWidth(), baseY - scale);
        window().fillRect(0, baseY + scale, window().getWidth(), window().getHeight() - baseY - scale);
    }

    void PlotterGUI::actionPerformed(GObservable* obj) {
        if (obj == mSave) {
            ofstream output(mFilename->getText());
            output << commands.str();
        }
    }

    void PlotterGUI::mousePressed(double mouseX, double mouseY) {
        commands << "PenUp" << endl;
        commands << "MoveAbs " << mouseToX(mouseX) << " " << mouseToY(mouseY) << endl;
        commands << "PenColor " << mColor->getText() << endl;
        commands << "PenWidth " << mWidth->getText() << endl;
        commands << "PenDown" << endl;

        lastX = mouseToX(mouseX);
        lastY = mouseToY(mouseY);
    }

    void PlotterGUI::mouseDragged(double mouseX, double mouseY) {
        double x = mouseToX(mouseX);
        double y = mouseToY(mouseY);

        commands << "MoveRel " << (x - lastX) << " " << (y - lastY) << endl;

        PenStyle style;
        style.color = mColor->getText();
        style.width = stringToDouble(mWidth->getText());
        addLine(lastX, lastY, x, y, style);


        lastX = x;
        lastY = y;

        requestRepaint();
    }

    double PlotterGUI::mouseToX(double x) const {
        return (x - baseX) / scale;
    }
    double PlotterGUI::mouseToY(double y) const {
        return -(y - baseY) / scale;
    }
}

GRAPHICS_HANDLER("Drawing Canvas", GWindow& window) {
    return make_shared<PlotterGUI>(window);
}
