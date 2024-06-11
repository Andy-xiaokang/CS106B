#pragma once

#include <string>

namespace MiniGUI {
    /* Families of available fonts. */
    enum class FontFamily {
        SERIF,
        SANS_SERIF,
        MONOSPACE,
        UNICODE_SERIF,
        UNICODE_SANS_SERIF,
        UNICODE_MONOSPACE
    };

    /* Font styles. */
    enum class FontStyle {
        NORMAL,
        BOLD,
        ITALIC,
        BOLD_ITALIC
    };

    /* Type representing a styled font, a combination of a font, font effects,
     * font size, and font color.
     *
     * Due to cross-platform concerns with certain fonts not being supported or
     * certain fonts being needed to meet certain requirements, code that renders
     * text should use this type to ensure no warnings are generated at runtime.
     *
     * This type is immutable.
     */
    class Font {
    public:
        /* Default font settings. */
        Font();
        Font(FontFamily family, FontStyle style, int size, const std::string& color);

        /* Access attributes, or produce a new font with the specified property. */
        FontFamily family() const;
        Font family(FontFamily family) const;

        FontStyle style() const;
        Font style(FontStyle style) const;

        int size() const;
        Font size(int size) const;

        std::string color() const;
        Font color(const std::string& color) const;

        /* Raw font string, used for communication with StanfordCPPLib. This
         * is primarily for communication with other types in MiniGUI and is
         * not meant to be used by the client.
         */
        std::string stanfordCPPLibFontString() const;

    private:
        FontFamily  mFamily;
        FontStyle   mStyle;
        int         mSize;
        std::string mColor;
    };
}
