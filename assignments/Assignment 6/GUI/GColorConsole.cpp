#include "GColorConsole.h"
#include "gthread.h"
#include "strlib.h"
using namespace std;

namespace {
    /* HTML header and footer.*/
    const string kHTMLHeader = R"(
         <html>
            <head></head>
            <body style="background-color:white;color:black;">
                <pre>)";

    const string kHTMLFooter = R"(</pre>
            </body>
        </html>
    )";
}

GColorConsole::GColorConsole() : ostream(new ConsoleStreambuf(this)) {
    // Handled in initializer list
}

GColorConsole::~GColorConsole() {
    delete rdbuf();
}

/* Clears everything. This will not update the display until updateDisplay is called. */
void GColorConsole::clearDisplay() {
    /* Kick out any remaining contents, then clear out the contents. */
    flushBuffer();
    mContents.clear();
}

/* Whenever the streambuf is sync'ed, take the buffer contents and push them
 * into the list of contents.
 */
int GColorConsole::ConsoleStreambuf::sync() {
    mOwner->updateDisplay();
    return 0;
}

/* Kicks out all text stashed in the streambuf and adds it to the list of text items
 * to display.
 */
void GColorConsole::flushBuffer() {
    /* Get the current contents of the buffer. If it's empty, we don't need to do
     * anything.
     */
    auto* buffer = static_cast<ConsoleStreambuf *>(rdbuf());

    auto contents = buffer->str();
    if (contents.empty()) return;

    /* Otherwise, clear the buffer and append this text. */
    buffer->str("");
    mContents.emplace_back(mStyle, contents);
}

/* Syncs to the display.
 *
 * TODO: This takes time O(n) to construct the string to display. I doubt that's going to
 * be a huge problem given that we already have to do O(n) work to copy the characters
 * over to the display. Update this if it gets too slow?
 */
void GColorConsole::updateDisplay() {
    /* Flush anything in our buffer to make sure the contents array holds
     * everything we need.
     */
    flushBuffer();

    /* Write all the contents. */
    stringstream toShow;
    toShow << kHTMLHeader;

    for (const auto& line: mContents) {
        /* Introduce the style. */
        toShow << "<span style=\"";
        toShow << "color:" << line.first.color << ";";
        if (line.first.fontStyle & BOLD)   toShow << "font-weight:bold;";
        if (line.first.fontStyle & ITALIC) toShow << "font-style:italic;";
        toShow << "font-size:" << line.first.fontSize.size() << "pt;";
        toShow << "\">";

        /* Write the text, escaping everything as needed. */
        toShow << htmlEncode(line.second);

        /* Close the style. */
        toShow << "</span>";
    }

    /* Close it out. */
    toShow << kHTMLFooter;

    /* Change text contents and scroll down. */
    GThread::runOnQtGuiThread([&, this] {
        readTextFromFile(toShow);
        scrollToBottom();
    });
}

void GColorConsole::setStyle(const string& color, FontStyle style, FontSize size) {
    flushBuffer();
    mStyle.color = color;
    mStyle.fontStyle = style;
    mStyle.fontSize = size;
}

GColorConsole::FontStyle GColorConsole::style() const {
    return mStyle.fontStyle;
}
string GColorConsole::color() const {
    return mStyle.color;
}
FontSize GColorConsole::fontSize() const {
    return mStyle.fontSize;
}

void GColorConsole::doWithStyle(const string& newColor, FontStyle newStyle, FontSize newSize, std::function<void ()> fn) {
    auto oldColor = color();
    auto oldStyle = style();
    auto oldSize  = fontSize();

    setStyle(newColor, newStyle, newSize);

    /* Execute the given callback. If it throws, undo all our changes before returning. */
    try {
        fn();
    } catch (...) {
        setStyle(oldColor, oldStyle);
        throw;
    }

    /* Hey, we succeeded! Roll back the styling. */
    setStyle(oldColor, oldStyle, oldSize);
}

void GColorConsole::doWithStyle(const string& color, FontStyle style, std::function<void ()> fn) {
    doWithStyle(color, style, fontSize(), fn);
}

void GColorConsole::doWithStyle(const string& color, std::function<void ()> fn) {
    doWithStyle(color, style(), fontSize(), fn);
}

void GColorConsole::doWithStyle(FontStyle style, std::function<void ()> fn) {
    doWithStyle(color(), style, fontSize(), fn);
}

void GColorConsole::doWithStyle(FontStyle style, FontSize size, std::function<void()> fn) {
    doWithStyle(color(), style, size, fn);
}
void GColorConsole::doWithStyle(const std::string& color, FontSize size, std::function<void()> fn) {
    doWithStyle(color, style(), size, fn);
}
void GColorConsole::doWithStyle(FontSize size, std::function<void()> fn) {
    doWithStyle(color(), style(), size, fn);
}

/**** FontSize implementation. ****/
FontSize::FontSize(size_t size): mSize(size) {
    // Handled in initialization list
}

size_t FontSize::size() const {
    return mSize;
}
