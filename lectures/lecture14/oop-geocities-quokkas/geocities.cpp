// Geocities-inspired page rendering.
//
// Author: Sean Szumlanski
// Date: Autumn 2023
//
// This library renders little Quokka profile pages in geocities-inspired themes
// with tiled backgrounds and bright Comic Sans font.

#include <iostream>
#include "geocities.h"
#include "gwindow.h"
#include "quokka.h"
#include "random.h"
using namespace std;

// Tiles the entirety of the given window with the provided background image.
void bgTile(GWindow& window, GImage& bg)
{
    // How many times must we repeat the image in the X and Y directions?
    int numY = ceil(window.getHeight() / bg.getHeight());
    int numX = ceil(window.getWidth() / bg.getWidth());

    // Current (x, y) coordinates where tile image should be drawn.
    int curX = 0;
    int curY = 0;

    // Tile time. :)
    for (int i = 0; i < numY; i++)
    {
        for (int j = 0; j < numX; j++)
        {
            window.draw(bg, curX, curY);
            curX += bg.getWidth();
        }
        curX = 0;
        curY += bg.getHeight();
    }
}

// Displays quokka profile in window with geocities-inspired theme.
void renderGeocitiesPage(Quokka *q)
{
    // Window width is based on size of quokka pic, so we load this up front.
    GImage quokkaImg("resources/quokkas/" + q->profilePic());

    // Create window. Initial position on screen is random to add to chaotic
    // vibe.
    int borderPadding = 80;
    GWindow* window = new GWindow(quokkaImg.getWidth() + borderPadding* 1.5,
                                  quokkaImg.getHeight() + borderPadding * 2.0);
    window->setTitle(q->getName());
    window->setLocation(randomInteger(0, SCREEN_WIDTH / 2),
                        randomInteger(0, SCREEN_HEIGHT / 2));
    window->setExitOnClose(true);
    window->setResizable(false);
    window->clear();

    // Load background image and tile window with image.
    int whichBackground = randomInteger(1, NUM_BACKGROUNDS);
    string bgFilename = BG_BASE_FILENAME +
                        integerToString(whichBackground) +
                        BG_FILE_EXTENSION;
    GImage bgimg(bgFilename);
    bgTile(*window, bgimg);

    // Quokka image placement.
    quokkaImg.sendToFront();
    quokkaImg.setCenterLocation(window->getWidth() / 2,
                                window->getHeight() / 2);

    // Draw rectangle before quokka so quokka image has black border around it.
    window->setColor("black");
    window->setFillColor("black");
    window->fillRect(quokkaImg.getX() - 2, quokkaImg.getY() - 2,
                     quokkaImg.getWidth() + 3, quokkaImg.getHeight() + 3);

    // Render the quokka.
    window->draw(quokkaImg);

    int titleFontSize = 30;
    int captionFontSize = 15;
    int fontWeight = QFont::ExtraBold;
    QFont titleFont(FONT_NAME, titleFontSize, fontWeight);
    QFont captionFont(FONT_NAME, captionFontSize, fontWeight);

    // Bright tacky yellow for font color.
    window->setColor("#FFFF00");

    // Print quokka name. There is honestly a lot of trial and error with the
    // string placement here. This is not the best way to try to get text
    // centered. See this quarter's programming assignments for examples of
    // how to center text more precisely.
    double horizontalScalar = 0.30;
    window->setFont(titleFont);
    int stringX = window->getWidth() / 2 -
                  horizontalScalar * q->getName().length() * titleFontSize;
    int stringY = (window->getHeight() -
                   quokkaImg.getHeight()) / 4.0 + 0.5 * titleFontSize;
    window->drawString(q->getName(), stringX, stringY);

    // Size of pepper image is used to determine caption placement below.
    GImage pepper("resources/img/pepper.png");

    // Print caption (information about quokka's adorableness rating). Again,
    // this is not the best way to go about trying to center these strings.
    window->setFont(captionFont);
    string footer = "How adorable:  " + integerToString(q->howAdorable());
    int footerX = (pepper.getWidth() + 10) / 2.0 + window->getWidth() / 2 -
                  horizontalScalar * footer.length() * captionFontSize;
    int footerY = window->getHeight() - (window->getHeight() -
                  quokkaImg.getHeight()) / 4.0 + 0.5 * captionFontSize;
    window->drawString(footer, footerX, footerY);

    // Display the pepper.
    int pepperPadding = 10;
    pepper.setX(footerX - pepper.getWidth() * 1.5 - pepperPadding);
    pepper.setY(footerY - captionFontSize / 2.0 - pepper.getHeight() / 2.0);
    window->draw(pepper);

    // Display window to user.
    window->setVisible(true);
}
