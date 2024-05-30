#ifndef GUIUtils_Included
#define GUIUtils_Included

#include "TemporaryComponent.h"
#include "Font.h"
#include "gobjects.h"
#include "ginteractors.h"
#include "gwindow.h"
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <functional>

/* Line breaking mode. You can select between breaking at spaces or
 * preserving all tokens as-is.
 */
enum class LineBreak {
    BREAK_SPACES,
    NO_BREAK_SPACES
};

/* Type: TextRender
 *
 * Object capable of rendering nicely-formatted text on the screen.
 */
class TextRender {
public:
    /* Bounding rectangles for the text render. The first option gives the bounds that were
     * provided at construction, which is useful if you want to make use of the full space
     * available. The second is the bounding rectangle for where the text will actually
     * draw, which is useful if you want to, say, draw a background behind just the part
     * used.
     */
    GRectangle bounds() const;
    GRectangle computedBounds() const;

    /* Intended font and actual font. */
    MiniGUI::Font font() const;
    MiniGUI::Font computedFont() const;

    /* Draws this text render in the specified window. */
    void draw(GWindow& window);
    void draw(GCanvas* canvas);

    /* Changes the horizontal text alignment. */
    void alignLeft();
    void alignCenterHorizontally();

    /* Changes the vertical text alignment. */
    void alignTop();
    void alignCenterVertically();

    /* Lays out a string to fit within the specified rectangle. The text might not fill up the
     * entirety of that rectangle if it isn't long enough.
     *
     * This function will attempt to respect the font size specified in the font string, but may
     * have to shrink the font size to get things to fit.
     */
    static std::shared_ptr<TextRender> construct(const std::string& text,
                                                 const GRectangle& bounds,
                                                 const MiniGUI::Font& font,
                                                 LineBreak breakMode = LineBreak::BREAK_SPACES);

private:
    TextRender() = default;
    GRectangle    mBounds;         // Bounds specified by the user
    GRectangle    mComputedBounds; // Bounds actually occupied.
    MiniGUI::Font mFont;          // Font specified by the user
    MiniGUI::Font mComputedFont;  // Font actually used

    /* Type representing a single line of text. */
    struct TextLine {
        std::string text;
        GPoint      baseline; // Where the baseline is
        double      width;
    };

    std::vector<TextLine> mLines;

    /* Breaks the text apart into lines, shrinking the font until it fits in the given rectangle.
     * Returns true on success.
     */
    bool fitText(const std::vector<std::string>& tokens, LineBreak breakMode);
};

/* Type: LegendRender
 *
 * Object capable of rendering a nicely-formatted chart legend on the screen.
 */
class LegendRender {
public:
    /* As before, there are two sets of bounds. The first is the bounds given in which the
     * legend should be constructed. The second is what's actually occupied.
     */
    GRectangle bounds() const;
    GRectangle computedBounds() const;

    void draw(GWindow& window);

    /* Constructs a legend render in which each text string has the same color.
     *
     * The input lists of strings may be shorter than the input list of colors, but must
     * not be longer.
     */
    static std::shared_ptr<LegendRender> construct(const std::vector<std::string>& strings,
                                                   const std::vector<std::string>& colors,
                                                   const GRectangle& bounds,
                                                   const MiniGUI::Font& font,
                                                   const std::string& borderColor,
                                                   LineBreak breakMode = LineBreak::BREAK_SPACES);

    /* Constructs a legend render in which each text string has an assigned colors
     *
     * The input lists of strings may be shorter than the input list of colors, but must
     * not be longer.
     *
     * The specified font will have all its attributes preserved except color, which
     * will change from item to item.
     */
    static std::shared_ptr<LegendRender> construct(const std::vector<std::string>& strings,
                                                   const std::vector<std::string>& bulletColors,
                                                   const GRectangle& bounds,
                                                   const std::vector<std::string>& textColors,
                                                   const MiniGUI::Font& font,
                                                   const std::string& borderColor,
                                                   LineBreak breakMode = LineBreak::BREAK_SPACES);

private:
    LegendRender() = default;
    GRectangle mBounds;
    GRectangle mComputedBounds;
    std::vector<std::shared_ptr<TextRender>> mLines;
    std::vector<std::string> mBulletColors;
    std::string mBorderColor;
};

/* Type: LineGraphRender
 *
 * Object capable of rendering a nicely-formatted line graph on the screen.
 */
class LineGraphRender {
public:
    GRectangle bounds() const;

    void draw(GWindow& window);

    /* Assembles the line graph renderer.
     *
     * The X and Y axes will assign labels to major tick marks. You can leave those labels
     * blank if you'd like, but the number of labels will be inferred from the lengths of
     * the label lists.
     *
     * You can have minor tick marks on each of the axes if you'd like by setting the
     * major tick size to a number greater than zero.
     *
     * The data points provided should be normalized to a virtual space where (0, 0) is the
     * origin and (1, 1) is the upper-right corner of the line graph. The lines don't have
     * to have the same number of points or pass perfectly over the tick marks on the X
     * axis.
     */
    static std::shared_ptr<LineGraphRender> construct(const std::vector<std::vector<GPoint>>& lines,
                                                      const std::vector<std::string>& xLabels,
                                                      const std::vector<std::string>& yLabels,
                                                      std::size_t xMinorTicks,
                                                      std::size_t yMinorTicks,
                                                      const GRectangle& bounds,
                                                      const MiniGUI::Font& xLabelFont,
                                                      const MiniGUI::Font& yLabelFont,
                                                      const std::vector<std::string>& lineColors,
                                                      const std::string& axisColors);

private:
    LineGraphRender() = default;

    /* Axis start and end points. */
    GPoint mOrigin, mXEnd, mYEnd;

    /* Line labels and fonts. */
    std::vector<std::string> mXLabels, mYLabels;
    MiniGUI::Font mXLabelFont, mYLabelFont;

    /* Axis parameters. */
    std::string mAxisColor;
    std::size_t mXMinorTicks, mYMinorTicks;

    /* The lines, in real rather than virtual space. */
    std::vector<std::vector<GPoint>> mLines;
    std::vector<std::string> mLineColors;

    GRectangle mBounds;

    /* Draws each component. */
    void drawXAxis(GWindow& window) const;
    void drawYAxis(GWindow& window) const;
    void drawLines(GWindow& window) const;
};

/* Clears the display, resetting it to a specific color. */
void clearDisplay(GWindow& window, const std::string& backgroundColor);
void clearDisplay(GCanvas* canvas, const std::string& backgroundColor);

/* Given a coordinate on the Earth's surface, along with a centering coordinate, returns
 * the (x, y) coordinates at which to display that coordinate in a Mollweide projection.
 * The returned coordinates are in logical space ([-2, +2] x [-1, +1]) and will need to
 * be converted to screen space before being displayed.
 *
 * The input longitudes and latitudes should be in degrees, not radians.
 */
std::tuple<double, double> mollweideProjectionOf(double latitude, double longitude,
                                                 double centerLongitude = 0.0,
                                                 double centerLatitude  = 0.0);

/* Creates a slider bar in the SOUTH console that slides over a given year range. */
Temporary<GSlider> makeYearSlider(GWindow& window, int startYear, int endYear);

/* Creates, but does not install, a combo box containing all the files in a directory
 * that pass the given filter, using the specified default.
 */
GComboBox* makeFileSelector(const std::string& baseDir,
                            const std::string& defaultOption,
                            std::function<bool(const std::string&)> filter);

/* Given a bounding box and an aspect ratio, returns the largest GRectangle
 * that (1) fits within the bounding box and (2) has that aspect ratio.
 */
GRectangle fitToBounds(const GRectangle& bounds, double aspectRatio);

#endif
