/******************************************************************************
 * File: Plotter.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Types and functions used by the Plotter assignment. You should not modify
 * this file when working through the assignment.
 */

#ifndef Plotter_Included
#define Plotter_Included

#include <istream>
#include <string>

/* Type: PenStyle
 *
 * Structure that contains information about a pen's properties. You should
 * not modify this structure. If you want to store more information about the
 * pen, you'll need to find another way to do so.
 */
struct PenStyle {
    double width;
    std::string color;
};



/**
 * Draws a line between the two indicated points using the specified pen
 * style.
 * <p>
 * Any part of the line that isn't in the box [-1, 1] x [-1, 1] will not
 * be drawn.
 *
 * @param x0    The x coordinate of where the line begins.
 * @param y0    The y coordinate of where the line begins.
 * @param x1    The x coordinate of where the line ends.
 * @param y1    The y coordinate of where the line ends.
 * @param style Information about the pen's current style.
 */
void drawLine(double x0, double y0, double x1, double y1, PenStyle info);



/**
 * Runs the plotter script contained in the given input stream. This function
 * assumes the input script is properly formatted and makes no guarantees about
 * what will happen if this isn't the case.
 *
 * @param input The input stream containing the script to run.
 */
void runPlotterScript(std::istream& input);

#endif
