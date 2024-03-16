// Geocities-inspired page rendering.
//
// Author: Sean Szumlanski
// Date: Autumn 2023
//
// This library renders little Quokka profile pages in geocities-inspired themes
// with tiled backgrounds and bright Comic Sans font.

#ifndef GEOCITIES_H
#define GEOCITIES_H

#include "quokka.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

// Chilanka is lovely, too, if available.
#define FONT_NAME "Comic Sans MS"

// Number of tile background images in resources/bg.
#define NUM_BACKGROUNDS 7

// Base filename (with path) for background images.
#define BG_BASE_FILENAME "resources/bg/bg"

// File extension for background images.
#define BG_FILE_EXTENSION ".jpg"

void renderGeocitiesPage(Quokka *q);

#endif // GEOCITIES_H
