#include "GUI/MiniGUI.h"
#include "GVector.h"
#include "Sierpinski.h"
using namespace std;
using namespace MiniGUI;

namespace {
    const string kBackgroundColor = "#FFFFFF";

    /* Slider options. */
    const int kLowOrder     = 0;
    const int kHighOrder    = 8;
    const int kDefaultOrder = 0;

    /* Header information. */
    const string kHeader =
        "Use the slider to change the order of the "
        "triangle. Drag the red ovals to reposition the corners.";

    const Font kHeaderFont(FontFamily::SERIF, FontStyle::ITALIC, 15, "#36454F"); // Charcoal
    const double kHeaderHeight = 100;
    const double kHeaderPadding   = 10;

    /* Control points. */
    const string kControlPointFillColor    = "#C41E3A"; // Cardinal
    const string kControlPointBorderColor  = "#620F1C"; // Half cardinal
    const double kControlPointRadius  = 10;
    const double kControlPointPadding = 25;

    /* Returns the center point of a GOval. */
    GPoint centerOf(const GOval* oval) {
        return {
            oval->getX() + oval->getWidth()  / 2.0,
            oval->getY() + oval->getHeight() / 2.0
        };
    }

    GRectangle expand(const GRectangle& rect, double delta) {
        return {
            rect.x - delta, rect.y - delta,
            rect.width + 2 * delta, rect.height + 2 * delta
        };
    }
    GRectangle expand(double x, double y, double width, double height, double delta) {
        return expand({ x, y, width, height }, delta);
    }

    /* Problem handler to visualize the Sierpinski triangles of different orders. */
    class SierpinskiGUI: public ProblemHandler {
    public:
        SierpinskiGUI(GWindow& window);

        void changeOccurredIn(GObservable* source) override;
        void mousePressed(double x, double y) override;
        void mouseDragged(double x, double y) override;
        void windowResized() override;

        static std::string name();

    protected:
        void repaint() override;

    private:
        Temporary<GSlider> mOrderSlider;
        int  mOrder;

        /* Current corner points. */
        GOval* mCornerPoints[3];

        /* Last mouse pushdown point. */
        GPoint mLastMouse;

        /* Last selected oval. */
        GOval* mSelected = nullptr;

        /* Workspace area where the triangle can be displayed. */
        GRectangle mWorkspace;

        /* Given an {x, y} point, clamps it into the workspace. */
        GPoint clampToWorkspace(double x, double y) const;

        void calculateGeometry();
    };

    SierpinskiGUI::SierpinskiGUI(GWindow& window) : ProblemHandler(window) {
        mOrderSlider = Temporary<GSlider>(new GSlider(kLowOrder, kHighOrder, kDefaultOrder),
                                          window,
                                          "SOUTH");
        mOrderSlider->setPaintLabels(true);
        mOrderSlider->setPaintTicks(true);
        mOrderSlider->setSnapToTicks(true);
        mOrderSlider->setMajorTickSpacing(1);
        mOrderSlider->setBounds(0, 0, window.getCanvasWidth(), mOrderSlider->getHeight());

        mOrder = kDefaultOrder;

        calculateGeometry();

        /* Set the control points to the window corners. */
        GPoint corners[3];
        corners[0] = { mWorkspace.x,                        mWorkspace.y + mWorkspace.height };
        corners[1] = { mWorkspace.x + mWorkspace.width,     mWorkspace.y + mWorkspace.height };
        corners[2] = { mWorkspace.x + mWorkspace.width / 2, mWorkspace.y                     };

        for (size_t i = 0; i < 3; i++) {
            mCornerPoints[i] = new GOval(corners[i].x - kControlPointRadius,
                                         corners[i].y - kControlPointRadius,
                                         2 * kControlPointRadius,
                                         2 * kControlPointRadius);
            mCornerPoints[i]->setFilled(true);
            mCornerPoints[i]->setColor(kControlPointBorderColor);
            mCornerPoints[i]->setFillColor(kControlPointFillColor);
            window.add(mCornerPoints[i]);
        }
    }

    void SierpinskiGUI::changeOccurredIn(GObservable* source) {
        if (source == mOrderSlider) {
            /* See if the value changed. If so, redraw things. */
            int order = mOrderSlider->getValue();
            if (order != mOrder) {
                mOrder = order;
                requestRepaint();
            }
        }
    }

    void SierpinskiGUI::repaint() {
        /* We only need to draw the header text once. */
        clearDisplay(window(), kBackgroundColor);

        auto header = TextRender::construct(kHeader, {
                                                kHeaderPadding, kHeaderPadding,
                                                window().getCanvasWidth()  - 2 * kHeaderPadding,
                                                window().getCanvasHeight() - 2 * kHeaderPadding
                                            }, kHeaderFont);
        header->alignCenterHorizontally();
        header->draw(window());

        /* Redraw the triangle. */
        auto p0 = centerOf(mCornerPoints[0]);
        auto p1 = centerOf(mCornerPoints[1]);
        auto p2 = centerOf(mCornerPoints[2]);
        drawSierpinskiTriangle(window(),
                               p0.x, p0.y,
                               p1.x, p1.y,
                               p2.x, p2.y,
                               mOrder);
    }

    void SierpinskiGUI::mousePressed(double x, double y) {
        /* See if we hit anything. */
        GPoint mouse = {x, y};
        for (auto* oval: mCornerPoints) {
            if (magnitudeOf(centerOf(oval) - mouse) <= kControlPointRadius) {
                mSelected = oval;
                mLastMouse = clampToWorkspace(mouse.x, mouse.y);
                return;
            }
        }

        /* Oops - didn't hit anything. */
        mSelected = nullptr;
    }

    void SierpinskiGUI::mouseDragged(double x, double y) {
        /* If nothing is selected, there's nothing to do. */
        if (mSelected == nullptr) return;

        /* Shift the selected oval over by the delta between our current and last
         * point.
         */
        GPoint mouse = clampToWorkspace(x, y);
        auto delta = mouse - mLastMouse;

        mSelected->move(delta.x, delta.y);
        requestRepaint();

        mLastMouse = mouse;
    }

    GPoint SierpinskiGUI::clampToWorkspace(double x, double y) const {
        x = max(min(x, mWorkspace.x + mWorkspace.width),  mWorkspace.x);
        y = max(min(y, mWorkspace.y + mWorkspace.height), mWorkspace.y);

        return { x, y };
    }

    void SierpinskiGUI::calculateGeometry() {
        /* Set up the control points. First, see how much space we'll need to draw the header. */
        auto headerArea = TextRender::construct(kHeader, {
                                                    kHeaderPadding, kHeaderPadding,
                                                    window().getCanvasWidth()  - 2 * kHeaderPadding,
                                                    kHeaderHeight,
                                                }, kHeaderFont)->computedBounds();

        mWorkspace = expand(0, headerArea.y + headerArea.height,
                            window().getCanvasWidth(),
                            window().getCanvasHeight() - headerArea.y - headerArea.height,
                            -kControlPointPadding);

        /* If we're out of space, use whatever is below the header. */
        if (mWorkspace.height < 0) mWorkspace.height = 0;
    }

    void SierpinskiGUI::windowResized() {
        calculateGeometry();

        /* Shift the corner points to fit inside the window, in case it shrank. */
        for (int i = 0; i < 3; i++) {
            double dx = (mCornerPoints[i]->getX() + mCornerPoints[i]->getWidth())  - (mWorkspace.x + mWorkspace.width);
            double dy = (mCornerPoints[i]->getY() + mCornerPoints[i]->getHeight()) - (mWorkspace.y + mWorkspace.height);

            if (dx < 0) dx = 0;
            if (dy < 0) dy = 0;

            mCornerPoints[i]->move(-dx, -dy);
        }

        ProblemHandler::windowResized();
    }
}

GRAPHICS_HANDLER("Interactive Sierpinski", GWindow& window) {
    return make_shared<SierpinskiGUI>(window);
}
