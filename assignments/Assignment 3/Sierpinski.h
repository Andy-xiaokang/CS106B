#ifndef Sierpinski_Included
#define Sierpinski_Included

#include "gwindow.h"

/**
 * Draws a Sierpinski triangle with the specified corners.
 *
 * An order-0 Sierpinski triangle is just a regular filled triangle. An order-n Sierpinski
 * triangle, for n > 0, consists of three smaller Sierpinski triangles of order n-1, each half
 * as wide and as tall as the original triangle, positioned so that they meet corner-to-corner.
 *
 * The order of the provided points is irrelevant.
 *
 * If the order provided to this function is negative, this function should call error() to report
 * an error.
 *
 * @param window The window in which to draw the triangle.
 * @param x0, y0 The x and y coordinates of the first corner of the triangle.
 * @param x1, y1 The x and y coordinates of the second corner of the triangle.
 * @param x2, y2 The x and y coordinates of the third corner of the triangle.
 * @param order  The order of the triangle, which will always be nonnegative.
 */
void drawSierpinskiTriangle(GWindow& window,
                            double x0, double y0,
                            double x1, double y1,
                            double x2, double y2,
                            int order);

#endif
