#include "GUIUtils.h"
#include "strlib.h"
#include "filelib.h"
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

namespace {
    /* Multiplier of the line height to use between natural linebreaks. */
    const double kLineSpacing = 1.1;

    /* Multiplier of the line height to use between paragraph breaks. */
    const double kParagraphSpacing = kLineSpacing + 0.35;

    /* Tokenizes a string into individually-renderable units. */
    vector<string> tokenize(const string& text) {
        istringstream input(text);

        string current;
        vector<string> result;

        while (true) {
            int next = input.get();

            /* If we hit EOF, flush the current token and stop. */
            if (next == EOF) {
                if (!current.empty()) result.push_back(current);
                return result;
            }

            /* If we hit a space, flush the current token and start a new one. */
            if (isspace(char(next))) {
                if (!current.empty()) result.push_back(current);
                current = string(1, char(next));
            }
            /* Otherwise, we have a non-space character. Extend the current token, flushing
             * it if it's a whitespace token.
             */
            else {
                if (!current.empty() && isspace(current[0])) {
                    result.push_back(current);
                    current.clear();
                }

                current += char(next);
            }
        }
    }

    /* Given a font string, reduces the size of that font by one unit. */
    bool reduceFont(MiniGUI::Font& font) {
        /* Can't reduce the font size below one. */
        if (font.size() <= 1) {
            return false;
        }
        font = font.size(font.size() - 1);
        return true;
    }
}

/***************************/
/******* TextRender ********/
/***************************/

shared_ptr<TextRender> TextRender::construct(const string& text,
                                             const GRectangle& bounds,
                                             const MiniGUI::Font& font,
                                             LineBreak breakMode) {
    /* Split the text apart into individual tokens. */
    auto tokens = tokenize(text);

    /* Keep trying to get things to fit, shrinking the font until we succeed.
     *
     * TODO: This is a silly O(n)-time algorithm. Use binary search instead?
     */
    shared_ptr<TextRender> result(new TextRender());
    result->mBounds         = bounds;
    result->mFont           = font;
    result->mComputedFont   = font;
    while (true) {
        /* See if this works. */
        if (result->fitText(tokens, breakMode)) break;

        /* Try reducing the font size. If we can't, then there's nothing to render, and
         * that's okay!
         */
        if (!reduceFont(result->mComputedFont)) {
            result->mComputedBounds = {
                result->mBounds.x, result->mBounds.y, 0, 0
            };
            break;
        }
    }

    return result;
}

/* Attempts to line wrap the text to fit it within the bounds rectangle. On success, returns true and
 * fills in the outparameters with information about the render. Otherwise, returns false and leaves
 * them unmodified.
 */
bool TextRender::fitText(const vector<string>& tokens, LineBreak breakMode) {
    /* Set up a rendering label. */
    GText label("");
    label.setFont(mComputedFont.stanfordCPPLibFontString());

    /* Compute the line height from the font metrics. */
    double lineHeight = label.getFontAscent() + label.getFontDescent();

    /* Track the original starting coordinates. We'll need this for later when we compute
     * the bounding box.
     */
    double renderedWidth = 0;
    double renderedHeight = 0;

    /* Initial position is in the upper-left corner, shifted down by the font ascent
     * (since we draw on the baseline.)
     */
    double x = mBounds.x;
    double y = mBounds.y + label.getFontAscent();

    vector<TextLine> computedLines;
    TextLine currLine;
    currLine.baseline = { x, y };
    currLine.width    = 0;

    /* Begin laying out tokens. */
    for (const auto& token: tokens) {
        /* If this is newline, insert a hard line break. */
        if (breakMode == LineBreak::BREAK_SPACES && token == "\n") {
            /* Cache the line generated so far. */
            computedLines.push_back(currLine);

            /* Shift down to the next line. */
            x = mBounds.x;
            y += lineHeight * kParagraphSpacing;

            currLine.text = "";
            currLine.width = 0;
            currLine.baseline = { x, y };
        }
        /* Otherwise, if this is another sort of whitespace token, scoot over as appropriate,
         * but don't adjust the bounding rectangle.
         */
        else if (breakMode == LineBreak::BREAK_SPACES && isspace(token[0])) {
            /* Don't do anything if we're at the front of a line. */
            if (int(x - mBounds.x) != 0) {
                label.setText(token);
                currLine.text += token;
                currLine.width += label.getWidth();
                x += label.getWidth();
            }
        }
        /* Otherwise, draw this token and update the global bounding rectangle. */
        else {
            label.setText(token);

            /* If we don't fit, advance to the next line. */
            if (breakMode == LineBreak::BREAK_SPACES && x + label.getWidth() > mBounds.x + mBounds.width) {
                /* Write out the line we have so far. */
                computedLines.push_back(currLine);

                x = mBounds.x;
                y += lineHeight * kLineSpacing;

                currLine.text = "";
                currLine.width = 0;
                currLine.baseline = { x, y };
            }

            currLine.text += token;

            /* Shift over by the width of what we just added. */
            x += label.getWidth();
            currLine.width += label.getWidth();

            /* Update our bounding rectangle. */
            renderedWidth  = max(renderedWidth, x - mBounds.x);
            renderedHeight = max(y + label.getFontDescent() - mBounds.y, renderedHeight);

            /* If we're out of bounds, report an error. */
            if (renderedWidth > mBounds.width || renderedHeight > mBounds.height) return false;
        }
    }

    /* Process any remaining tokens. */
    if (!currLine.text.empty()) {
        computedLines.push_back(currLine);
    }

    mLines = computedLines;
    mComputedBounds = { mBounds.x, mBounds.y, renderedWidth, renderedHeight };
    return true;
}

void TextRender::draw(GWindow& window) {
    draw(window.getCanvas());
}

void TextRender::draw(GCanvas* canvas) {
    GText label;
    label.setFont(mComputedFont.stanfordCPPLibFontString());
    label.setColor(mComputedFont.color());

    for (const auto& line: mLines) {
        label.setLocation(line.baseline);
        label.setText(line.text);
        canvas->draw(&label);
    }
}

GRectangle TextRender::bounds() const {
    return mBounds;
}

GRectangle TextRender::computedBounds() const {
    return mComputedBounds;
}

MiniGUI::Font TextRender::font() const {
    return mFont;
}

MiniGUI::Font TextRender::computedFont() const {
    return mComputedFont;
}

void TextRender::alignLeft() {
    /* For each line, shift that line to the border of the initial boundary. */
    for (auto& line: mLines) {
        line.baseline = {
            mBounds.x,
            line.baseline.y
        };
    }
}

void TextRender::alignCenterHorizontally() {
    /* For each line, shift that line to be centered in the initial boundary. */
    for (auto& line: mLines) {
        line.baseline = {
            mBounds.x + (mBounds.width - line.width) / 2.0,
            line.baseline.y
        };
    }
}

void TextRender::alignTop() {
    /* Adjust each line so that its relative distance to the top of the full bounds
     * matches its distance to the top of the computed bounds.
     */
    for (auto& line: mLines) {
        line.baseline = {
            line.baseline.x,
            mBounds.y + (line.baseline.y - mComputedBounds.y)
        };
    }

    /* Adjust the computed bounds as well. */
    mComputedBounds = {
        mComputedBounds.x,
        mBounds.y,
        mComputedBounds.width,
        mComputedBounds.height
    };
}

void TextRender::alignCenterVertically() {
    /* Figure out where the new computed bounds should start. */
    double newY = mBounds.y + (mBounds.height - mComputedBounds.height) / 2.0;

    /* Adjust each line's Y coordinate so that its distance from the new Y coordinate matches
     * its distance from the computed bounds.
     */
    for (auto& line: mLines) {
        line.baseline = {
            line.baseline.x,
            newY + (line.baseline.y - mComputedBounds.y)
        };
    }

    /* Adjust the computed bounds as well. */
    mComputedBounds = {
        mComputedBounds.x,
        newY,
        mComputedBounds.width,
        mComputedBounds.height
    };
}

/***************************/
/****** LegendRender *******/
/***************************/

namespace {
    const double kBulletSize    = 10;
    const double kBulletPadding = 10;
    const double kItemPadding   = 5;
}

shared_ptr<LegendRender> LegendRender::construct(const vector<string>& strings,
                                                 const vector<string>& colors,
                                                 const GRectangle& bounds,
                                                 const vector<string>& textColors,
                                                 const MiniGUI::Font& font,
                                                 const string& borderColor,
                                                 LineBreak breakMode) {
    /* Validate input. */
    if (strings.size() > colors.size()) error("Not enough colors to draw legend.");

    /* Set up initial fields. */
    shared_ptr<LegendRender> result(new LegendRender());
    result->mBounds = bounds;
    result->mBorderColor = borderColor;
    result->mBulletColors = colors;

    /* Attempt to use the current font size to make everything work. If it does work, great! If not, back off
     * the font size and try again.
     *
     * TODO: This uses a silly O(n)-time algorithm. Switch to binary search?
     */
    MiniGUI::Font currFont = font;
    while (true) {
        /* Current x and y coordinates, for the purposes of laying out text. */
        double x = bounds.x;
        double y = bounds.y + kItemPadding;

        vector<shared_ptr<TextRender>> entries;

        /* TODO: Short-circuit if we fail early? */
        const double bulletSpacing = kBulletSize + 2 * kBulletPadding;
        double width = 0;
        for (size_t i = 0; i < strings.size(); i++) {
            /* Construct a text render for this item, tracking where it ended up. */


            /* Give unbounded vertical space. We'll do adjustments for height later on. */
            auto render = TextRender::construct(strings[i], {
                                                    x + bulletSpacing, y,
                                                    bounds.width - bulletSpacing - kItemPadding,
                                                    numeric_limits<double>::max()
                                                }, currFont.color(textColors[i]), breakMode);

            y = render->computedBounds().y + render->computedBounds().height + kItemPadding;
            entries.push_back(render);

            width = max(width, render->computedBounds().width);
        }

        /* Get the net bounding box. We'll force this to be as wide as the space that we're given
         * for consistency's sake.
         */
        GRectangle computedBounds = {
            bounds.x, bounds.y, bounds.width, y - bounds.y
        };

        /* Did we fit? */
        if (computedBounds.height <= bounds.height && computedBounds.width <= bounds.width) {
            result->mComputedBounds = computedBounds;
            result->mLines = entries;
            return result;
        }

        /* Oops, didn't fit. Shrink the font and try again. */
        if (!reduceFont(currFont)) {
            result->mComputedBounds = {
                result->mBounds.x, result->mBounds.y, 0, 0
            };
            return result;
        }
    }
}

shared_ptr<LegendRender> LegendRender::construct(const vector<string>& strings,
                                                 const vector<string>& colors,
                                                 const GRectangle& bounds,
                                                 const MiniGUI::Font& font,
                                                 const string& borderColor,
                                                 LineBreak breakMode) {
    return construct(strings, colors, bounds, vector<string>(strings.size(), font.color()), font, borderColor, breakMode);
}

void LegendRender::draw(GWindow& window) {
    /* First, draw all the text items and their corresponding bullets. */
    for (size_t i = 0; i < mLines.size(); i++) {
        mLines[i]->draw(window);

        /* Draw the bullet, vertically-centered. */
        window.setColor(mBulletColors[i]);
        GRectangle bullet = {
            mComputedBounds.x + kBulletPadding,
            mLines[i]->computedBounds().y + mLines[i]->computedBounds().height / 2.0 - kBulletSize / 2.0,
            kBulletSize,
            kBulletSize,
        };
        window.fillRect(bullet);
    }

    /* Draw the overall bounding box. */
    window.setColor(mBorderColor);
    window.drawRect(mComputedBounds);
}

GRectangle LegendRender::bounds() const {
    return mBounds;
}

GRectangle LegendRender::computedBounds() const {
    return mComputedBounds;
}

/***************************/
/***** LineGraphRender *****/
/***************************/

namespace {
    /* Axis line constants. */
    const double kAxisLineWidth = 2;
    const double kSmallTickSize = 8;
    const double kLargeTickSize = 16;

    /* Padding between tick marks and labels. */
    const double kTickPadding = 2;

    /* Plotted line constants. */
    const double kPlottedLineWidth = 5;

    /* Given a list of labels and a label font, returns a bounding rectangle big
     * enough to hold any of those labels.
     */
    GDimension labelDimensionsFor(const vector<string>& labels,
                                  const MiniGUI::Font& font) {
        GText measurer;
        measurer.setFont(font.stanfordCPPLibFontString());

        /* Find the maximum extents across all labels. */
        double maxWidth = 0;
        double maxHeight = 0;

        for (const auto& label: labels) {
            measurer.setLabel(label);

            /* For height, have to factor in ascent and descent. */
            maxHeight = max(maxHeight, measurer.getFontAscent() + measurer.getFontDescent());
            maxWidth = max(maxWidth, measurer.getWidth());
        }

        return { maxWidth, maxHeight };
    }

    /* Given all the X and Y axis labels and their fonts, returns the three control points
     * (origin, x end, y end) that define the axes
     */
    tuple<GPoint, GPoint, GPoint>
    axesFor(const GRectangle& bounds,
            const vector<string>& xLabels,
            const vector<string>& yLabels,
            const MiniGUI::Font& xLabelFont,
            const MiniGUI::Font& yLabelFont) {

        double originX = bounds.x;
        double originY = bounds.y + bounds.height;

        double xEnd    = bounds.x + bounds.width;
        double yEnd    = bounds.y;

        /* The labels are drawn centered on all the vertical and horizontal tick marks.
         * That means that half of a label will hang off in each of the X and Y directions on
         * each endpoint of the axis, which means that we need to buffer the full width of
         * one x-axis label on the x-axis and one full y-axis label on the y-axis.
         */
        auto xSize = labelDimensionsFor(xLabels, xLabelFont);
        auto ySize = labelDimensionsFor(yLabels, yLabelFont);
        xEnd    -= xSize.width  / 2;
        yEnd    += ySize.height / 2;

        /* Additionally, we need to account for the width of the y-axis labels and the
         * height of the x-axis labels, since they're drawn to the left of and underneath
         * the y-axis, respectively.
         *
         * We also need to consider the possibility that there's a large half-label hanging
         * off from the origin point on either the X or Y axis.
         */
        originX += max(ySize.width,  xSize.width  / 2.0);
        originY -= max(xSize.height, ySize.height / 2.0);

        /* Don't forget tick mark padding! */
        originX += kTickPadding;
        originY -= kTickPadding;

        /* Finally, we have to buffer half of a tick mark in each direction, since those
         * ticks overdraw the lines.
         */
        originX += kLargeTickSize / 2.0;
        originY -= kLargeTickSize / 2.0;

        return make_tuple(GPoint(originX, originY), GPoint(xEnd, originY), GPoint(originX, yEnd));
    }
}

shared_ptr<LineGraphRender>
LineGraphRender::construct(const vector<vector<GPoint>>& lines,
                           const vector<string>& xLabels,
                           const vector<string>& yLabels,
                           size_t xMinorTicks,
                           size_t yMinorTicks,
                           const GRectangle& bounds,
                           const MiniGUI::Font& xLabelFont,
                           const MiniGUI::Font& yLabelFont,
                           const vector<string>& lineColors,
                           const string& axisColor) {
    /* Boundary case: We can't draw a line graph if we don't have at least two ticks
     * in each of the X and Y directions.
     */
    if (xLabels.size() < 2 || yLabels.size() < 2) error("Insufficiently many ticks.");

    /* Boundary case: We need one color per line. */
    if (lineColors.size() < lines.size()) error("Too few line colors (have " + to_string(lineColors.size()) + ", need " + to_string(lines.size()) + ").");

    shared_ptr<LineGraphRender> result(new LineGraphRender());

    /* Copy over basic information. */
    result->mXLabelFont  = xLabelFont;
    result->mYLabelFont  = yLabelFont;
    result->mXLabels     = xLabels;
    result->mYLabels     = yLabels;
    result->mAxisColor   = axisColor;
    result->mLineColors  = lineColors;
    result->mXMinorTicks = xMinorTicks;
    result->mYMinorTicks = yMinorTicks;
    result->mBounds      = bounds;

    /* First order of business: figure out how much space we need for the labels
     * on the axes. This is necessrary in order for us to figure out where the
     * axes go.
     */
    tie(result->mOrigin, result->mXEnd, result->mYEnd) =
            axesFor(bounds, xLabels, yLabels, xLabelFont, yLabelFont);

    /* Now that we have these coordinate points, we can remap the lines so that they use
     * the line graph coordinate space instead of the virtual [0, 1] coordinate space.
     */
    double baseX  = result->mOrigin.x;
    double baseY  = result->mOrigin.y;
    double width  = result->mXEnd.x   - result->mOrigin.x;
    double height = result->mOrigin.y - result->mYEnd.y;
    for (const auto& line: lines) {
        vector<GPoint> physical;
        for (const auto& pt: line) {
            physical.push_back({ baseX + width * pt.x, baseY - height * pt.y });
        }
        result->mLines.push_back(physical);
    }

    return result;
}

void LineGraphRender::drawYAxis(GWindow& window) const {
    /* Draw the axis line. */
    GLine axisLine(mOrigin, mYEnd);
    axisLine.setColor(mAxisColor);
    axisLine.setLineWidth(kAxisLineWidth);
    window.draw(&axisLine);

    /* Draw tick marks. */
    GText tickLabel("");
    tickLabel.setColor(mAxisColor);
    tickLabel.setFont(mYLabelFont.stanfordCPPLibFontString());

    /* Useful constants. */
    const double height  = mOrigin.y - mYEnd.y;
    const double spacing = height / (mYLabels.size() - 1);

    for (size_t i = 0; i < mYLabels.size(); i++) {
        /* Major tick mark. */
        double y = mOrigin.y - i * spacing;
        double x = mOrigin.x;
        window.setColor(mAxisColor);
        window.drawLine(x - kLargeTickSize / 2.0, y, x + kLargeTickSize / 2.0, y);

        /* Tick mark label. */
        tickLabel.setLabel(mYLabels[i]);
        window.draw(&tickLabel, x - kLargeTickSize / 2.0 - kTickPadding - tickLabel.getWidth(), y + tickLabel.getFontDescent());

        /* Minor tick marks. */
        if (i + 1 != mYLabels.size()) {
            for (size_t tick = 0; tick < mYMinorTicks; tick++) {
                /* Curious where this comes from? We're going to pretend that we're drawing minor
                 * tick marks on top of the major tick marks to get the spacing right.
                 */
                double minorY = y - (tick + 1) * (spacing / (mYMinorTicks + 1));
                window.drawLine(x - kSmallTickSize / 2.0, minorY, x + kSmallTickSize / 2.0, minorY);
            }
        }
    }
}

void LineGraphRender::drawXAxis(GWindow& window) const {
    /* Draw the axis line. */
    GLine axisLine(mOrigin, mXEnd);
    axisLine.setColor(mAxisColor);
    axisLine.setLineWidth(kAxisLineWidth);
    window.draw(&axisLine);

    /* Draw tick marks. */
    GText tickLabel("");
    tickLabel.setColor(mAxisColor);
    tickLabel.setFont(mXLabelFont.stanfordCPPLibFontString());

    /* Useful constants. */
    const double width  = mXEnd.x - mOrigin.x;
    const double spacing = width / (mXLabels.size() - 1);

    for (size_t i = 0; i < mXLabels.size(); i++) {
        double x = mOrigin.x + i * spacing;
        double y = mOrigin.y;

        window.setColor(mAxisColor);
        window.drawLine(x, y - kLargeTickSize / 2.0, x, y + kLargeTickSize / 2.0);

        /* Tick mark label. */
        tickLabel.setLabel(mXLabels[i]);
        window.draw(&tickLabel, x - tickLabel.getWidth() / 2.0, y + kLargeTickSize / 2.0 + kTickPadding + tickLabel.getFontAscent());

        /* Minor tick marks. */
        if (i + 1 != mXLabels.size()) {
            for (size_t tick = 0; tick < mXMinorTicks; tick++) {
                /* Curious where this comes from? We're going to pretend that we're drawing minor
                 * tick marks on top of the major tick marks to get the spacing right.
                 */
                double minorX = x + (tick + 1) * (spacing / (mXMinorTicks + 1));
                window.drawLine(minorX, y - kSmallTickSize / 2.0, minorX, y + kSmallTickSize / 2.0);
            }
        }
    }
}

void LineGraphRender::drawLines(GWindow& window) const {
    /* Draw lines from back to front. */
    for (size_t j = mLines.size(); j > 0; --j) {
        GLine line(0, 0, 0, 0);
        line.setLineWidth(kPlottedLineWidth);
        line.setColor(mLineColors[j - 1]);

        for (size_t i = 0; i + 1 < mLines[j - 1].size(); i++) {
            auto src = mLines[j - 1][i];
            auto dst = mLines[j - 1][i + 1];
            line.setStartPoint(src.x, src.y);
            line.setEndPoint(dst.x, dst.y);
            window.draw(&line);
        }
    }
}

GRectangle LineGraphRender::bounds() const {
    return mBounds;
}

void LineGraphRender::draw(GWindow& window) {
    drawXAxis(window);
    drawYAxis(window);
    drawLines(window);
}




/***************************/
/*******  Helpers   ********/
/***************************/

void clearDisplay(GWindow& window, const std::string& backgroundColor) {
    clearDisplay(window.getCanvas(), backgroundColor);
}
void clearDisplay(GCanvas* canvas, const std::string& backgroundColor) {
    canvas->setColor(backgroundColor);
    canvas->fillRect({ 0, 0, canvas->getWidth(), canvas->getHeight() });
}

/* The math behind this function is shamelessly lifted (with love) from Wikipedia:
 *
 *   https://en.wikipedia.org/wiki/Mollweide_projection#Mathematical_formulation
 *
 * The inverse formula is way easier than the forward formula. :-)
 *
 * The output of this function is a pair of real numbers in the range [-2, +2] x [-1, 1],
 * which then needs to be scaled and translated based on the window size.
 */
tuple<double, double> mollweideProjectionOf(double latitude, double longitude,
                                            double longitudeOffset, double latitudeOffset) {
    /* Adjust longitude to fit map. */
    longitude -= longitudeOffset;
    if (longitude < -180) longitude += 360;
    if (longitude > 180)  longitude -= 360;

    latitude -= latitudeOffset;
    if (latitude < -90)   latitude += 180;
    if (latitude >  90)   latitude  -= 180;

    /* Convert from degrees (what we get back from USGS) to radians. */
    longitude *= M_PI / 180;
    latitude  *= M_PI / 180;

    /* There isn't a closed-form solution to work out the coordinates, so we'll
     * use Newton's method to try to get close to one. Thanks, calculus!
     */
    static const size_t kNumIterations = 100;
    double theta = latitude;
    for (size_t i = 0; i < kNumIterations; i++) {
        theta = theta - (2 * theta + sin(2 * theta) - M_PI * sin(latitude)) / (2 + 2 * cos(2 * theta));
    }

    /* Armed with theta, we can work out the x and y coordinates. */
    double x = 2 * cos(theta) * longitude / M_PI;
    double y = sin(theta);

    return make_tuple(x, y);
}

Temporary<GSlider> makeYearSlider(GWindow& window, int startYear, int endYear) {
    Temporary<GSlider> result(new GSlider(startYear, endYear, startYear),
                              window,
                              "SOUTH");
    result->setPaintLabels(true);
    result->setPaintTicks(true);
    result->setSnapToTicks(true);
    result->setMajorTickSpacing(10);
    result->setMinorTickSpacing(1);
    result->setBounds(0, 0, window.getCanvasWidth(), result->getHeight());
    return result;
}

namespace {
    /* Given a filename, returns the filename without its suffix. */
    string trimExtensionFrom(const string& filename) {
        auto index = filename.rfind('.');
        return index == string::npos? filename : filename.substr(0, index);
    }
}

/* Utility to create a file chooser combo box. */
GComboBox* makeFileSelector(const string& baseDir, const string& defaultOption,
                            function<bool(const string&)> predicate) {
    /* Stash files here. */
    Vector<string> files;

    GComboBox* result = new GComboBox();
    result->addItem(defaultOption);
    for (const string& file: listDirectory(baseDir)) {
        if (predicate(file)) {
            files += file;
        }
    }

    /* Sort files. This comparator drops off suffixes when doing the comparison, hence
     * the stable sort (everything is already sorted by name).
     */
    stable_sort(files.begin(), files.end(), [](const string& lhs, const string& rhs) {
        return trimExtensionFrom(lhs) < trimExtensionFrom(rhs);
    });

    /* Sort everything. */
    for (string file: files) {
        result->addItem(file);
    }

    result->setEditable(false);
    return result;
}

/* Fit bounds to aspect ratio. */
GRectangle fitToBounds(const GRectangle& bounds, double aspectRatio) {
    double width, height;

    /* Too narrow? */
    if (bounds.width / bounds.height <= aspectRatio) {
        width = bounds.width;
        height = width / aspectRatio;
    } else {
        height = bounds.height;
        width = height * aspectRatio;
    }

    double baseX = bounds.x + (bounds.width  - width)  / 2.0;
    double baseY = bounds.y + (bounds.height - height) / 2.0;

    return { baseX, baseY, width, height };
}
