#include "GUI/MiniGUI.h"
#include "Demos/GVector.h"
#include "HumanPyramids.h"
#include "simpio.h"
#include <cmath>
#include <algorithm>
#include <vector>
#include <limits>
#include <sstream>
#include <iomanip>
using namespace std;
using namespace MiniGUI;

namespace {
    /* Color palette. */
    const string kBackgroundColor = "#727472"; // Nickel
    const string kLatticeColor    = "#C46210"; // Alloy Orange

    /* Colors for people are based on how much weight is on top of them. */
    struct ColorEntry {
        double minWeight;
        int red, green, blue;
    };

    /* Colors by weight. Entries are interpolated between boundaries. */
    const vector<ColorEntry> kPersonColors = {
        /* No weight? You get pure green. */
        {  0.0, 0x00, 0xFF, 0x00 },

        /* Some weight? You get yellow. */
        {  400, 0xFF, 0xFF, 0x00 },

        /* Lots of weight? You get orange. */
        {  600, 0xFF, 0x80, 0x00 },

        /* Painful weight? You get yellow. */
        {  600, 0xFF, 0x00, 0x00 },

        /* Tons of weight? You get darker and darker. */
        { 1000, 0x80, 0x00, 0x00 },
        { 5000, 0x00, 0x00, 0x00 },

        /* This last entry must be bigger than all possible weights. */
        { numeric_limits<double>::infinity(), 0x00, 0x00, 0x00 },
    };

    /* Amount to back off colors when drawing borders of the people in the pyramid. */
    const double kBorderDamper = 0.75;

    /* Slider constants. */
    const int kMinHeight     = 1;
    const int kMaxHeight     = 31;
    const int kDefaultHeight = 6;
    const int kMajorTickSize = 5;

    /* Radius of a single person. We may have to shrink things a bit if not everything is
     * going to fit, but this is our preferred size.
     */
    const double kPersonRadius = 20;

    /* Everyone is arranged in an equilateral triangle lattice. This is the preferred side
     * length for one of those triangles, which we might not be able to do if not everything
     * fits.
     */
    const double kLatticeSize  = 60;
    const double kLatticeWidth = 5;

    /* Height of an equilateral triangle with unit side length. */
    const double kEquilateralTriangleHeight = sqrt(3.0) / 2.0;

    /* Padding around the border of the window. */
    const double kPadding = 75;

    /* Bounding box for drawing person information. */
    const double kPersonInfoX           = 10;
    const double kPersonInfoY           = 10;
    const double kPersonInfoWidth       = 400;
    const double kPersonInfoHeight      = 100;
    const double kPersonInfoPadding     = 10;
    const string kPersonInfoFillColor   = "#F5FFFA"; // Mint Cream
    const string kPersonInfoBorderColor = "#F0EAD6"; // Eggshell

    const Font kPersonInfoFont(FontFamily::SANS_SERIF, FontStyle::NORMAL, 24, kBackgroundColor);

    /* Given information about the number of layers in the pyramid, returns the bounding
     * box into which that pyramid should be drawn, along with the scale factor to use
     * when drawing.
     */
    tuple<GRectangle, double> computeGraphicsParameters(const GRectangle& bounds, int height) {
        /* Compute the total height we'll need. The triangle lattice height is completely
         * independent of the number of people, and the size of a person only factors in
         * when we look at the amount overhanging from the top and bottom. As a result, the
         * height of the lattice is just the total number of layers, times the height of one
         * of the triangles, plus the height of one person.
         *
         * The -1 term on the height is here because the height counts the number of layers,
         * and there's a fencepost issue in doing so.
         */
        double figureHeight = (height - 1) * kEquilateralTriangleHeight * kLatticeSize + 2 * kPersonRadius;

        /* The width is computed similarly, but it's a bit easier because the width of
         * an equilateral triangle is easier to determine.
         */
        double figureWidth = (height - 1) * kLatticeSize + 2 * kPersonRadius;

        /* If the figure height exceeds the working height, rescale everything. */
        double scaleFactor = min(1.0, bounds.height / figureHeight);
        if (scaleFactor < 1) {
            figureHeight *= scaleFactor;
            figureWidth  *= scaleFactor;
        }

        /* Determine how to center everything. */
        double x = bounds.x + (bounds.width  - figureWidth)  / 2.0;
        double y = bounds.y + (bounds.height - figureHeight) / 2.0;

        /* Assemble the result. */
        GRectangle result = { x, y, figureWidth, figureHeight };
        return make_tuple(result, scaleFactor);
    }

    /* Draws the lines that make up the lattice in which the people sit. The inputs are
     * provided so that the first two points form one side of the triangle and the third
     * point is the other point on the triangle, such that the third point is sandwiched
     * between p0 and p1, like this:
     *
     *
     *        p2
     *        /\
     *       /  \
     *     p0 -- p1
     */
    void drawLatticeLines(GWindow& window,
                          const GPoint& p0, const GPoint& p1, const GPoint& p2,
                          int height) {
        /* Edge case! If the height is 1, there's nothing to draw. */
        if (height == 1) return;

        /* We're going to draw the lattice by connecting p0 and p1, then moving upward
         * toward p2 by a step of the appropriate size. That will require us to get
         * vectors that move us toward p2.
         */
        GVector p02 = (p2 - p0) / (height - 1);
        GVector p12 = (p2 - p1) / (height - 1);

        /* Draw the lines! */
        GLine line(0, 0, 0, 0);
        line.setColor(kLatticeColor);
        line.setLineWidth(kLatticeWidth);

        for (int i = 0; i < height; i++) {
            auto src = p0 + p02 * i;
            auto dst   = p1 + p12 * i;

            line.setStartPoint(src);
            line.setEndPoint  (dst);

            window.draw(&line);
        }
    }

    /* Given information about a lattice, returns the control points for that lattice
     * (the three bounding points of the triangle lattice). The points are arranged so
     * that p0 is on top and the bottom has p1 on the left and p2 on the right.
     */
    tuple<GPoint, GPoint, GPoint> controlPointsFor(const GRectangle& bounds,
                                                   int height,
                                                   double latticeSize, double personRadius) {
        /* Determine the corners of the lattice, factoring in the buffer we need to leave in
         * for the people.
         */
        double topY    = bounds.y + personRadius;
        double bottomY = bounds.y + bounds.height - personRadius;
        double width   = latticeSize * (height - 1);

        GPoint p0 { bounds.x + bounds.width / 2, topY };
        GPoint p1 { bounds.x + (bounds.width  - width) / 2, bottomY };
        GPoint p2 { bounds.x + (bounds.width + width) / 2, bottomY };

        return make_tuple(p0, p1, p2);
    }

    /* Draws the lattice (everyone's arms). */
    void drawLattice(GWindow& window, int height, const GRectangle& bounds,
                     double latticeSize, double personRadius) {

        GPoint p0, p1, p2;
        tie(p0, p1, p2) = controlPointsFor(bounds, height, latticeSize, personRadius);

        /* In graphics space, the triangle looks like this:
         *
         *           p0
         *          /  \
         *         /    \
         *        p1 -- p2
         *
         */
        drawLatticeLines(window, p0, p2, p1, height);
        drawLatticeLines(window, p1, p0, p2, height);
    }

    /* Given a weight on someone's back, returns the color that person should receive.
     * This works by linearly interpolating between a pair of endpoints in the color table.
     */
    tuple<int, int, int> colorFor(double weight) {
        /* Scan forward until we find a boundary that exceeds us. We'll use linear search
         * because the number of colors is low.
         */
        size_t index = 1;
        while (kPersonColors[index].minWeight < weight) {
            index++;
        }

        /* For simplicity. */
        const auto& lhs = kPersonColors[index - 1];
        const auto& rhs = kPersonColors[index];

        /* Linearly interpolate between the colors. */
        double alpha = (weight - lhs.minWeight) / (rhs.minWeight - lhs.minWeight);

        int red   = lhs.red   + (rhs.red   - lhs.red  ) * alpha;
        int green = lhs.green + (rhs.green - lhs.green) * alpha;
        int blue  = lhs.blue  + (rhs.blue  - lhs.blue ) * alpha;

        return tie(red, green, blue);
    }

    /* Draws a single person in the human pyramid. A graphics coordinate is provided so
     * we know where to draw, and the logical coordinate (row, col) is provided for
     * determining color information.
     */
    void drawPersonAt(GWindow& window, const GPoint& pt, int row, int col, int pyramidHeight, double radius) {
        int red, green, blue;
        tie(red, green, blue) = colorFor(weightOnBackOf(row, col, pyramidHeight));

        /* Background */
        window.setColor(GCanvas::createRgbPixel(red, green, blue));
        window.fillOval(pt.x - radius, pt.y - radius, 2 * radius, 2 * radius);

        /* Border */
        red   *= kBorderDamper;
        green *= kBorderDamper;
        blue  *= kBorderDamper;
        window.setColor(GCanvas::createRgbPixel(red, green, blue));
        window.drawOval(pt.x - radius, pt.y - radius, 2 * radius, 2 * radius);
    }

    /* Draws the people in the human pyramid. */
    void drawPeople(GWindow& window, int height, const GRectangle& bounds,
                    double latticeSize, double personRadius) {
        GPoint p0, p1, p2;
        tie(p0, p1, p2) = controlPointsFor(bounds, height, latticeSize, personRadius);

        /* We always draw a person at position p0. */
        drawPersonAt(window, p0, 0, 0, height, personRadius);

        /* Edge case: If the height is exactly 1, that's all we draw. */
        if (height == 1) return;

        /* Otherwise, get vectors from p0 to p1, which we use when moving down the
         * lattice, and from p1 to p2, which we use when moving across the lattice.
         * Scale them based on the height so that each of these vectors represent
         * moving one person over or down.
         */
        auto down = (p1 - p0) / (height - 1);
        auto left = (p2 - p1) / (height - 1);

        /* Draw a bunch of people! */
        for (int row = 1; row < height; row++) {
            for (int col = 0; col <= row; col++) {
                drawPersonAt(window, p0 + down * row + left * col, row, col, height, personRadius);
            }
        }
    }

    /* Draws a human pyramid within the given rectangle. */
    void drawPyramid(GWindow& window, int height,
                     const GRectangle& bounds, double scaleFactor) {
        /* Determine the actual person radius and lattice sizes to use. */
        double personRadius = kPersonRadius * scaleFactor;
        double latticeSize  = kLatticeSize  * scaleFactor;

        drawLattice(window, height, bounds, latticeSize, personRadius);
        drawPeople (window, height, bounds, latticeSize, personRadius);
    }

    /* Draws text in a box. */
    GRectangle drawTextBox(const string& text,
                           const GRectangle& bounds,
                           double padding,
                           const string& fillColor,
                           const string& borderColor,
                           const MiniGUI::Font& font,
                           GWindow& window) {
        auto render = TextRender::construct(text, {
                                                bounds.x + padding, bounds.y + padding,
                                                bounds.width - 2 * padding, bounds.height - 2 * padding
                                            }, font);

        /* Draw the box. */
        window.setColor(fillColor);
        window.fillRect(bounds);

        window.setColor(borderColor);
        window.drawRect(bounds);

        /* Draw the text. */
        render->draw(window);
        return bounds;
    }

    /* Problem handler that displays a variety of human pyramids and reports the weight on
     * each person.
     */
    class HumanPyramidsGUI: public ProblemHandler {
    public:
        HumanPyramidsGUI(GWindow& window);

        void changeOccurredIn(GObservable* source) override;
        void mouseMoved(double x, double y) override;
        void windowResized() override;

    protected:
        void repaint() override;

    private:
        /* Controls for the height of the pyramid. */
        Temporary<GSlider> mHeightSlider;
        int mHeight;

        /* Whether the pyramid itself is dirty. This happens if the height
         * changes.
         */
        bool mPyramidDirty = true;

        /* Which person, if any, is selected. We use the values of -1 as a
         * sentinel if no one is selected.
         */
        int mSelectedRow = -1, mSelectedCol = -1;

        /* Whether the selected person is dirty. This happens if the mouse moves around
         * in a way that changes things.
         */
        bool mPersonDirty = false;

        /* Last bounding rectangle for the selected person. */
        GRectangle mPersonInfo;

        /* Bounding rectangle for positioning everything. */
        GRectangle mBounds;

        /* Sets which person is selected. */
        void select(int row, int col);
        void clearSelection();

        /* Redraws selected graphics items. */
        void redrawPyramid(GWindow& window);
        void redrawPersonInfo(GWindow& window);

        void calculateGeometry();
    };


    HumanPyramidsGUI::HumanPyramidsGUI(GWindow& window) : ProblemHandler(window) {
        mHeightSlider = Temporary<GSlider>(new GSlider(kMinHeight, kMaxHeight, kDefaultHeight),
                                           window,
                                           "SOUTH");
        mHeightSlider->setPaintLabels(true);
        mHeightSlider->setPaintTicks(true);
        mHeightSlider->setSnapToTicks(true);
        mHeightSlider->setMajorTickSpacing(kMajorTickSize);
        mHeightSlider->setMinorTickSpacing(1);
        mHeightSlider->setBounds(0, 0, window.getCanvasWidth(), mHeightSlider->getHeight());

        mHeight = kDefaultHeight;

        calculateGeometry();
    }

    void HumanPyramidsGUI::changeOccurredIn(GObservable* source) {
        if (source == mHeightSlider) {
            /* See if the value changed. If so, redraw things. */
            int height = mHeightSlider->getValue();
            if (height != mHeight) {
                mHeight = height;
                mPyramidDirty = true;
                requestRepaint();
            }
        }
    }
    void HumanPyramidsGUI::repaint() {
        if (mPersonDirty) {
            redrawPersonInfo(window());
        }
        if (mPyramidDirty) {
            redrawPyramid(window());

            /* Pretend the person is dirty too - we need to redraw things. */
            mPersonDirty = true;
        }
    }

    void HumanPyramidsGUI::redrawPyramid(GWindow& window) {
        clearDisplay(window, kBackgroundColor);

        GRectangle bounds;
        double scaleFactor;
        tie(bounds, scaleFactor) = computeGraphicsParameters(mBounds, mHeight);

        drawPyramid(window, mHeight, bounds, scaleFactor);

        mPyramidDirty = false;
    }

    void HumanPyramidsGUI::redrawPersonInfo(GWindow& window) {
        /* Clear the area where we draw this information. */
        window.setColor(kBackgroundColor);
        window.fillRect(mPersonInfo);

        /* If someone is selected, display their information. */
        if (mSelectedRow != -1 && mSelectedCol != -1) {
            ostringstream builder;
            builder << "Person (" << mSelectedRow << ", " << mSelectedCol << ")" << endl;
            builder << "  Weight on back: "
                    << fixed << setprecision(2) << weightOnBackOf(mSelectedRow, mSelectedCol, mHeight) << endl;

            mPersonInfo = drawTextBox(builder.str(),
                                      {
                                          kPersonInfoX, kPersonInfoY,
                                          kPersonInfoWidth,
                                          kPersonInfoHeight,
                                      },
                                      kPersonInfoPadding,
                                      kPersonInfoFillColor,
                                      kPersonInfoBorderColor,
                                      kPersonInfoFont,
                                      window);
        }

        mPersonDirty = false;
    }

    void HumanPyramidsGUI::mouseMoved(double x, double y) {
        /* Get graphics information, which we'll need to invert everything. */

        GRectangle bounds;
        double scaleFactor;
        tie(bounds, scaleFactor) = computeGraphicsParameters(mBounds, mHeight);

        /* We'll need the sizes of the objects in the window in order to invert things. */
        double latticeSize  = kLatticeSize  * scaleFactor;
        double personRadius = kPersonRadius * scaleFactor;

        /* Get the control points, which we'll use for figuring out where we are. */
        GPoint p0, p1, p2;
        tie(p0, p1, p2) = controlPointsFor(bounds, mHeight, latticeSize, personRadius);

        /* Edge Case! If the height is exactly one, then we're overlapping the one person
         * precisely if we're within a radius of them.
         */
        GPoint mouse { x, y };
        if (mHeight == 1) {
            if (magnitudeOf(p0 - mouse) <= personRadius) {
                select(0, 0);
            } else {
                clearSelection();
            }
            return;
        }

        /* Get vectors going from p0 to p1 (down) and from p1 to p2 (left). The scale should
         * be chosen so that moving down or over one lattice unit corresponds to one unit in
         * the new space.
         */
        auto down = (p1 - p0) / (mHeight - 1);
        auto left = (p2 - p1) / (mHeight - 1);

        /* The down and left vectors form a basis for the lattice space. We want to
         * convert from canonical space to down/left space, which means we want to compute
         *
         *  | d    l | -1                             -1
         *  | o    e |     | x - p0 |     | d.x  l.x |    |x - p0|
         *  | w    f |     | y - p0 |  =  | d.y  l.y |    |y - p0|
         *  | n    t |
         *
         * That means that we want this matrix calculation:
         *
         *           1           |  l.y    -l.x | |x - p0|
         * --------------------  | -d.y     d.x | |y - p0|
         * d.x * ly - l.x * d.y
         *
         *         (det)               (A)            (v)
         */
        double det = down.x * left.y - left.x * down.y;

        auto v = mouse - p0;
        GPoint latticePoint = {
             (left.y * v.x - left.x * v.y) / det,
            (-down.y * v.x + down.x * v.y) / det
        };

        /* That lattice point is almost certainly real-valued. Round the coordinates to the
         * nearest integers. This will never get us into trouble because the closest point
         * to us is never going to involve rounding the opposite direction.
         *
         * Because of how we chose our matrix columns, our X coordinate corresponds to the
         * row number and our Y coordinate corresponds to the column.
         */
        int row = round(latticePoint.x);
        int col = round(latticePoint.y);

        /* If that point isn't a real person, we're done. */
        if (row < 0 || row >= mHeight || col < 0 || col > row) {
            clearSelection();
        } else {
            /* Now, see how far the mouse is from the circle at that point. */
            if (magnitudeOf(mouse - (p0 + down * row + left * col)) <= personRadius) {
                select(row, col);
            } else {
                clearSelection();
            }
        }
    }

    void HumanPyramidsGUI::select(int row, int col) {
        /* If this is the same as what we already have, there's nothing to do. */
        if (row == mSelectedRow && col == mSelectedCol) return;

        /* Otherwise, we need to redraw things. */
        mSelectedRow = row;
        mSelectedCol = col;
        mPersonDirty = true;
        requestRepaint();
    }

    void HumanPyramidsGUI::clearSelection() {
        select(-1, -1);
    }

    void HumanPyramidsGUI::windowResized() {
        calculateGeometry();
        ProblemHandler::windowResized();
    }

    void HumanPyramidsGUI::calculateGeometry() {
        double width  = max(window().getCanvasWidth()  - 2 * kPadding, 2 * kPadding);
        double height = max(window().getCanvasHeight() - 2 * kPadding, 2 * kPadding);

        /* The space we have to work with is the whole canvas, less the padding on each side. */
        mBounds = {
            (window().getCanvasWidth() - width) / 2.0, (window().getCanvasHeight() - height) / 2.0,
            width,
            height
        };

        /* Force everything to be redrawn. */
        mPyramidDirty = mPersonDirty = true;
    }
}

GRAPHICS_HANDLER("Human Pyramids", GWindow& window) {
    return make_shared<HumanPyramidsGUI>(window);
}

CONSOLE_HANDLER("Human Pyramids") {
    do {
        int row    = getInteger("Enter row of person to look up: ");
        int col    = getInteger("Enter column of person to look up: ");
        int height = getInteger("Enter number of rows in the pyramid: ");

        try {
            double result = weightOnBackOf(row, col, height);
            cout << "Weight on that person's back is " << result << endl;
        } catch (const ErrorException& e) {
            cout << "An error occurred. The error message is " << e.what() << endl;
        }

    } while (getYesOrNo("Want to look up another person? "));
}
