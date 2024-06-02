#include "Font.h"
#include "map.h"
using namespace std;

namespace {
    /* Serif font. */
    const char kSerifFont[] =
        #ifdef __APPLE__
            "Didot"
        #else
            "Serif"
        #endif
    ;

    /* Sans-serif font. */
    const char kSansSerifFont[] =
        #ifdef __APPLE__
            "Helvetica"
        #else
            "Sans Serif"
        #endif
    ;

    /* Monospace font. */
    const char kMonospaceFont[] =
        #ifdef __APPLE__
            "Monaco"
        #else
            "Monospace"
        #endif
    ;

    /* Unicode Serif font. */
    const char kUnicodeSerifFont[] =
        #ifdef __APPLE__
            "Times"
        #elif defined(_WIN32)
            "Times New Roman"
        #else
            "Serif"
        #endif
    ;

    /* Unicode Sans-Serif font. */
    const char kUnicodeSansSerifFont[] =
        #ifdef __APPLE__
            "Lucida Grande"
        #elif defined(_WIN32)
            "Lucida Sans Unicode"
        #else
            "Sans Serif"
        #endif
    ;

    /* Unicode monospace font. */
    const char kUnicodeMonospaceFont[] =
        #ifdef __APPLE__
            "Lucida Grande"
        #elif defined(_WIN32)
            "Lucida Sans Unicode"
        #else
            "Monospace"
        #endif
    ;


    /* Font family -> String */
    string to_string(MiniGUI::FontFamily family) {
        if (family == MiniGUI::FontFamily::MONOSPACE) {
            return kMonospaceFont;
        } else if (family == MiniGUI::FontFamily::SANS_SERIF) {
            return kSansSerifFont;
        } else if (family == MiniGUI::FontFamily::SERIF) {
            return kSerifFont;
        } else if (family == MiniGUI::FontFamily::UNICODE_MONOSPACE) {
            return kUnicodeMonospaceFont;
        } else if (family == MiniGUI::FontFamily::UNICODE_SANS_SERIF) {
            return kUnicodeSansSerifFont;
        } else if (family == MiniGUI::FontFamily::UNICODE_SERIF) {
            return kUnicodeSerifFont;
        } else {
            error("Unknown font family.");
        }
    }

    /* Font style -> StanfordCPPLib string. */
    string to_string(MiniGUI::FontStyle style) {
        if (style == MiniGUI::FontStyle::BOLD) {
            return "BOLD";
        } else if (style == MiniGUI::FontStyle::BOLD_ITALIC) {
            return "BOLDITALIC";
        } else if (style == MiniGUI::FontStyle::ITALIC) {
            return "ITALIC";
        } else if (style == MiniGUI::FontStyle::NORMAL) {
            return "<normal>";
        } else {
            error("Unknown font style.");
        }
    }
}

namespace MiniGUI {
    /* Defaults. */
    Font::Font() : mFamily(FontFamily::SANS_SERIF),
                   mStyle(FontStyle::NORMAL),
                   mSize(13),
                   mColor("black") {
        // Handled in initialization list.
    }

    Font::Font(FontFamily family, FontStyle style, int size, const string& color)
        : mFamily(family), mStyle(style), mSize(size), mColor(color) {
        // Handled in initialization list.
    }

    FontFamily Font::family() const {
        return mFamily;
    }
    Font Font::family(FontFamily family) const {
        auto result = *this;
        result.mFamily = family;
        return result;
    }

    FontStyle Font::style() const {
        return mStyle;
    }
    Font Font::style(FontStyle style) const {
        auto result = *this;
        result.mStyle = style;
        return result;
    }

    int Font::size() const {
        return mSize;
    }
    Font Font::size(int size) const {
        auto result = *this;
        result.mSize = size;
        return result;
    }

    string Font::color() const {
        return mColor;
    }
    Font Font::color(const string& color) const {
        auto result = *this;
        result.mColor = color;
        return result;
    }

    string Font::stanfordCPPLibFontString() const {
        /* Begin with the font name. */
        string result = to_string(family());

        /* Style, if appropriate. */
        if (style() != FontStyle::NORMAL) {
            result += "-" + to_string(style());
        }

        /* End with font size. */
        result += "-" + to_string(size());

        return result;
    }
}
