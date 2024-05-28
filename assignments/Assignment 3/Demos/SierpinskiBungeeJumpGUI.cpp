#include "GUI/MiniGUI.h"
#include "Sierpinski.h"
#include "GVector.h"
#include "gtimer.h"
#include <algorithm>
using namespace std;

namespace {
    const string kBackgroundColor = "#FFFFFF";
    const string kBorderColor     = "#4C516D"; // Independence

    /* Timing parameters. */
    const int    kFramesPerSecond = 100;
    const double kPauseTime = 1000.0 / kFramesPerSecond;
    const int    kFramesPerAnimation = 100;

    /* Triangle parameters. */
    const int kOrder = 8;

    /* Height of an equilateral triangle with unit side length. */
    const double kEquilateralHeight = sqrt(3.0) / 2.0;

    /* Computes the centroid of three points. */
    GPoint centroidOf(const GPoint& p0, const GPoint& p1, const GPoint& p2) {
        return {
            (p0.x + p1.x + p2.x) / 3.0,
            (p0.y + p1.y + p2.y) / 3.0
        };
    }


    /* Problem handler to do a deep dive into the Sierpinski triangle. */
    class SierpinskiBungeeJumpGUI: public ProblemHandler {
    public:
        SierpinskiBungeeJumpGUI(GWindow& window);
        ~SierpinskiBungeeJumpGUI();

        void timerFired() override;
        void windowResized() override;

    protected:
        void repaint() override;

    private:
        GTimer* mTimer;
        int    mFrame = 0; // Which animation frame we're on

        GPoint mPoints[3];       // Control points of the triangle
        GPoint mCentroid;        // Centroid of rotation

        void calculateGeometry();
    };

    SierpinskiBungeeJumpGUI::SierpinskiBungeeJumpGUI(GWindow& window) : ProblemHandler(window), mTimer(new GTimer(kPauseTime)) {
        mTimer->start();
        calculateGeometry();
    }

    SierpinskiBungeeJumpGUI::~SierpinskiBungeeJumpGUI() {
        /* TODO: There seems to be a bug in GTimer that causes major problems if we use an
         * actual GTimer object here rather than a pointer to one. Fix the leak and
         * remove this destructor.
         */
        mTimer->stop();
    }

    void SierpinskiBungeeJumpGUI::timerFired() {
        if (mTimer->isStarted()) {
            mFrame++;
            if (mFrame == kFramesPerAnimation) mFrame = 0;

            requestRepaint();
        }
    }

    void SierpinskiBungeeJumpGUI::repaint() {
        clearDisplay(window(), kBackgroundColor);

        /* See where we are in this animation. */
        double alpha = mFrame / double(kFramesPerAnimation);

        /* Imagine we have this starting triangle
         *
         *            p2
         *            /\
         *           /--\
         *          /\  /\
         *         /--\/--\
         *        /\      /\
         *       /--\   C/--\
         *      /\  /\  /\  /\
         *     /--\/--\/--\/--\
         *    p0      A^^^^B  p1
         *
         * We want to end up such that
         *
         *     point A ends up where p2 used to be,
         *     point B ends up where p0 used to be, and
         *     point C ends up where p1 used to be.
         *
         * This corresponds to
         *
         *     1. rotating the entire figure around the centroid of triangle ABC by 120 degrees, then
         *     2. doubling the size of the entire figure, centering around the the centroid of ABC.
         *
         * We'll compute the rotation and scale based on the progress that we've made so far.
         *
         * There's one last effect to take into account. If we purely zoom into the center of this
         * triangle, we'll end up looking in pure whitespace. We need to also slightly translate
         * everything over a bit; specifically, so that the triangle in the top becomes the new
         * center point.
         *
         * To do this, we'll compute the centroid of that new triangle and interpolate it over
         * into position.
         */

        GPoint sticky = mPoints[0];
        auto anchor = centroidOf(mPoints[0] + (sticky - mPoints[0]) / 2,
                                 mPoints[1] + (sticky - mPoints[1]) / 2,
                                 mPoints[2] + (sticky - mPoints[2]) / 2);


        /* Rotate everything around that point. */
        double theta = alpha * 2 * M_PI / 3;
        GPoint p0 = mCentroid + (rotate(mPoints[0] - mCentroid, theta));
        GPoint p1 = mCentroid + (rotate(mPoints[1] - mCentroid, theta));
        GPoint p2 = mCentroid + (rotate(mPoints[2] - mCentroid, theta));
        anchor    = mCentroid + (rotate(anchor     - mCentroid, theta));

        /* Scale everything around that point. */
        p0     += alpha * (p0     - mCentroid);
        p1     += alpha * (p1     - mCentroid);
        p2     += alpha * (p2     - mCentroid);
        anchor += alpha * (anchor - mCentroid);

        /* Determine how much to shift everything over based on the anchor point. */
        auto shift = (mCentroid - anchor) * alpha;

        p0 += shift;
        p1 += shift;
        p2 += shift;

        /* Draw our triangle! */
        drawSierpinskiTriangle(window(),
                               p0.x, p0.y,
                               p1.x, p1.y,
                               p2.x, p2.y,
                               kOrder);

        /* Draw the borders to avoid revealing the detail that when the rotation finishes, the
         * triangle isn't actually adjacent to bordering triangles in the same way that the original
         * one was. :-)
         */
        window().setColor(kBorderColor);

        /* Sides */
        window().fillRect(0, 0, mPoints[0].x, window().getCanvasHeight());
        window().fillRect(mPoints[1].x, 0, window().getCanvasWidth() - mPoints[1].x, window().getCanvasHeight());

        /* Top and bottom */
        window().fillRect(0, 0, window().getCanvasWidth(), mPoints[2].y);
        window().fillRect(0, mPoints[0].y, window().getCanvasWidth(), window().getCanvasHeight() - mPoints[0].y);
    }

    void SierpinskiBungeeJumpGUI::calculateGeometry() {
        /* We want our triangle to be an equilateral triangle that's as large as possible
         * while still fitting into the window. Compute the scale factor to use assuming we
         * have a unit triangle.
         */
        double scale  = min(window().getCanvasWidth(), window().getCanvasHeight() / kEquilateralHeight);
        double width  = 1.0 * scale;
        double height = kEquilateralHeight * scale;

        /* Use that to compute our origin points. */
        double baseX = (window().getCanvasWidth()  - width)  / 2.0;
        double baseY = (window().getCanvasHeight() - height) / 2.0;

        /* Compute the points of the corners of our triangle. */
        mPoints[0] = { baseX,             baseY + height };
        mPoints[1] = { baseX + width,     baseY + height };
        mPoints[2] = { baseX + width / 2, baseY          };

        mCentroid = centroidOf(mPoints[0], mPoints[1], mPoints[2]);
    }

    void SierpinskiBungeeJumpGUI::windowResized() {
        calculateGeometry();
        ProblemHandler::windowResized();
    }
}

GRAPHICS_HANDLER("Sierpinski Bungee Jump", GWindow& window) {
    return make_shared<SierpinskiBungeeJumpGUI>(window);
}
