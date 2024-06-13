#ifndef GColorConsole_Included
#define GColorConsole_Included

#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <memory>
#include "gbrowserpane.h"

/* Type representing a font size. */
class FontSize {
public:
    /* Default font size. */
    static constexpr std::size_t DEFAULT_FONT_SIZE = 11;

    explicit FontSize(std::size_t size = DEFAULT_FONT_SIZE);
    std::size_t size() const;

private:
    std::size_t mSize;
};

/* Type representing a graphical text console in which text can be rendered
 * in different colors and styles. The object itself is a stream so that you can
 * read and write using standard formatted operations, and independently it can
 * be installed in a GWindow as a GInteractor.
 */
class GColorConsole: public std::ostream, public GBrowserPane {
public:
    GColorConsole();
    ~GColorConsole();

    enum FontStyle {
        NORMAL = 0,
        BOLD   = 1,
        ITALIC = 2,
        BOLD_ITALIC = 3 // BOLD | ITALIC
    };

    /* Styling. */
    void setStyle(const std::string& color = "black",
                  FontStyle style = NORMAL,
                  FontSize  size = FontSize{});
    std::string color() const;
    FontStyle style() const;
    FontSize  fontSize() const;

    /* Clears all previously-rendered text. This will not update the display; to do that,
     * flush the stream afterwards..
     */
    void clearDisplay();

    /* Performs the specified block of code with the styling on the console changed
     * to the indicated values. This is useful if, say, you want to temporarily
     * change the color or font style.
     */
    void doWithStyle(const std::string& color, FontStyle style, std::function<void()> fn);
    void doWithStyle(const std::string& color, FontStyle style, FontSize size, std::function<void()> fn);
    void doWithStyle(FontStyle style, FontSize size, std::function<void()> fn);
    void doWithStyle(const std::string& color, FontSize size, std::function<void()> fn);
    void doWithStyle(FontSize size, std::function<void()> fn);
    void doWithStyle(const std::string& color, std::function<void()> fn);
    void doWithStyle(FontStyle style, std::function<void()> fn);

private:
    /* std::stringbuf subtype that happens to hold a reference back to the containing
     * object. Whenever anything gets synced, it dumps the contents to the hooked
     * display.
     */
    struct ConsoleStreambuf: public std::stringbuf {
        GColorConsole* mOwner;

        ConsoleStreambuf(GColorConsole* owner) : mOwner(owner) {}

        /* Updates the display. */
        virtual int sync() override;
    };

    /* Flushes any buffered text out of the stringbuf and into the list of contents.
     * This is often used to ensure that the styling is done properly.
     */
    void flushBuffer();

    /* Updates the display to sync it with the stashed text. */
    void updateDisplay();

    /* Blocks of text rendered so far, each tagged with a style. */
    struct Style {
        std::string color = "black";
        FontStyle   fontStyle = NORMAL;
        FontSize    fontSize = FontSize{};
    };

    Style mStyle;
    std::vector<std::pair<Style, std::string>> mContents;

    /* No copying, sorry! */
    GColorConsole(const GColorConsole &) = delete;
    GColorConsole(GColorConsole &&) = delete;
    void operator= (GColorConsole) = delete;
};

#endif
