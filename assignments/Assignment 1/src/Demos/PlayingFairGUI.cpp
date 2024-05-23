#include "../PlayingFair.h"
#include "../GUI/MiniGUI.h"
#include "ginteractors.h"
#include "simpio.h"
using namespace std;
using namespace MiniGUI;

namespace {
    /* Amount to nudge borders of the world coordinates to avoid degenerate cases. */
    const double kEpsilon = 0.01;

    /* Line thickness. */
    const double kLineWidth = 2;

    /* Margin from the border of the window to the actual crystal. */
    const double kWindowPadding = 20;

    /* How tall the text is allowed to be. */
    const double kFontHeight = 24;

    /* Slider controls. */
    const int kMinOrder = 0;
    const int kMaxOrder = 13;
    const int kDefaultOrder = 0;

    /* Foreground and background colors. */
    const Vector<string> kColors = {
        "#000000",
        "#FFFF00"
    };

    const Font kLabelFont(FontFamily::SERIF, FontStyle::NORMAL, 16, "#00FFFF");

    /* Type: Geometry
     *
     * Information about how big each cell should be and how to display things properly in the
     * window.
     */
    struct Geometry {
        double minX, minY;   // Lowest X and Y coordinate of any point
        double baseX, baseY; // Offset from the upper-left corner of the screen to the content
        double scale;        // Scaling factor converting from global to window coordinates
    };

    /* Given a crystal, computes the geometry used to draw it. */
    Geometry geometryFor(const Vector<GPoint>& points, GWindow& window) {
        /* Compute the bounding box. */
        double minX =  numeric_limits<double>::infinity();
        double minY =  numeric_limits<double>::infinity();

        double maxX = -numeric_limits<double>::infinity();
        double maxY = -numeric_limits<double>::infinity();

        /* Expand our box around the spots used in the fractal. */
        for (const auto& loc: points) {
            minX = min(minX, loc.x);
            minY = min(minY, loc.y);

            maxX = max(maxX, loc.x);
            maxY = max(maxY, loc.y);
        }

        /* Nudge everything ever so slightly to avoid degenerate cases. */
        minX -= kEpsilon;
        minY -= kEpsilon;

        maxX += kEpsilon;
        maxY += kEpsilon;

        /* Get the width and height of the world. */
        double width  = maxX - minX;
        double height = maxY - minY;

        /* Computing the scaling factors needed to scale to the window width and window height. */
        double scaleX = (window.getWidth()  - 2 * kWindowPadding) / width;
        double scaleY = (window.getHeight() - 2 * kWindowPadding) / height;
        double scale = min(scaleX, scaleY);

        /* Compute base x and y based on the scale. */
        double baseX = kWindowPadding + (window.getWidth()  - 2 * kWindowPadding - width  * scale) / 2.0;
        double baseY = kWindowPadding + (window.getHeight() - 2 * kWindowPadding - height * scale) / 2.0;

        return {
            minX, minY,
            baseX, baseY,
            scale
        };
    }

    Vector<GPoint> fractalOfOrder(int n) {
        Vector<GPoint> result;

        GPoint loc(0, 0);

        /* If the order is odd, everything is, by default, rotated 180 degrees and we need to correct for
         * this. If the order is even, it's rotated 30 degrees counterclockwise.
         */
        double theta;
        if (n == 0) theta = 0;
        else if (n % 2 == 0) theta = 11 * M_PI / 6;
        else theta = M_PI;

        result += loc;

        for (char ch: aSequenceOfOrder(n)) {
            if (ch == 'A') {
                loc = {
                    loc.x + cos(theta),
                    loc.y + sin(theta)
                };

                result += loc;
                theta += M_PI / 3.0;
            } else if (ch == 'B') {
                theta += M_PI;
            } else {
                error("Unknown character in A-sequence: '" + to_string(ch) + "'.");
            }
        }

        return result;
    }

    void drawFractal(int order, GWindow& window) {
        auto points = fractalOfOrder(order);

        window.setColor(kColors[0]);
        window.fillRect(0, 0, window.getCanvasWidth(), window.getCanvasHeight());

        auto g = geometryFor(points, window);

        /* For efficiency's sake, only make one GLine. */
        GLine line;
        line.setColor(kColors[1]);
        line.setLineWidth(kLineWidth);

        for (int i = 0; i + 1 < points.size(); i++) {
            double x0 = g.baseX + (points[i].x - g.minX) * g.scale;
            double y0 = g.baseY + (points[i].y - g.minY) * g.scale;

            double x1 = g.baseX + (points[i+1].x - g.minX) * g.scale;
            double y1 = g.baseY + (points[i+1].y - g.minY) * g.scale;

            line.setStartPoint(x0, y0);
            line.setEndPoint(x1, y1);
            window.draw(line);
        }

        /* Display the order, just for simplicity's sake. */
        auto render = TextRender::construct("Derived from order-" + to_string(order) + " A-sequence.",
                                            { kWindowPadding, window.getCanvasHeight() - kWindowPadding - kFontHeight, window.getCanvasWidth() - 2 * kWindowPadding, kFontHeight},
                                            kLabelFont, LineBreak::NO_BREAK_SPACES);
        render->alignCenterVertically();
        render->draw(window);
    }

    class PlayingFairGUI: public ProblemHandler {
    public:
        PlayingFairGUI(GWindow& window);
        void changeOccurredIn(GObservable* source) override;

    protected:
        void repaint() override;

    private:
        Temporary<GSlider> order;
        int lastOrder = kDefaultOrder;
    };

    PlayingFairGUI::PlayingFairGUI(GWindow& window) : ProblemHandler(window) {
        auto* slider = new GSlider(kMinOrder, kMaxOrder, kDefaultOrder);
        slider->setPaintLabels(true);
        slider->setPaintTicks(true);
        slider->setSnapToTicks(true);
        slider->setMajorTickSpacing(1);
        slider->setBounds(0, 0, window.getCanvasWidth(), slider->getHeight());

        order = Temporary<GSlider>(slider, window, "SOUTH");
    }

    void PlayingFairGUI::changeOccurredIn(GObservable* source) {
        if (source == order) {
            int currOrder = order->getValue();
            if (currOrder != lastOrder) {
                lastOrder = currOrder;
                requestRepaint();
            }
        }
    }

    void PlayingFairGUI::repaint() {
        drawFractal(lastOrder, window());
    }
}

GRAPHICS_HANDLER("Playing Fair", GWindow& window) {
    return make_shared<PlayingFairGUI>(window);
}

CONSOLE_HANDLER("Playing Fair") {
    do {
        int n = getInteger("Enter value of n: ");

        try {
            string aSequence = aSequenceOfOrder(n);
            cout << "A-sequence of order " << n << ": " << aSequence << endl;
        } catch (const ErrorException& e) {
            cerr << "An error occurred: " << e.what() << endl;
        }

        try {
            string bSequence = bSequenceOfOrder(n);
            cout << "B-sequence of order " << n << ": " << bSequence << endl;
        } catch (const ErrorException& e) {
            cerr << "An error occurred: " << e.what() << endl;
        }
    } while (getYesOrNo("Choose another order? "));
}
