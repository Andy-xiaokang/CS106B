#include <iostream>
#include <math.h>
#include "console.h"
#include "gtypes.h"
#include "gwindow.h"
using namespace std;

static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 800;
static const int BASE_Y = SCREEN_HEIGHT - SCREEN_HEIGHT * .35;
static const int BASE_LEFT_X = 170;
static const int BASE_RIGHT_X = SCREEN_WIDTH - 170;
static const double COS_60 = 0.5;
static const double SIN_60 = sqrt(3) * 0.5;

// Returns a GPoint that forms an equilateral triangle with the points passed as
// parameters.
// GPoint thirdEquilateralPoint(GPoint bottomLeft, GPoint otherPoint)
// {
//     double deltaX = otherPoint.x - bottomLeft.x;
//     double deltaY = otherPoint.y - bottomLeft.y;
//     double x = bottomLeft.x + (deltaX * COS_60 + deltaY * SIN_60);
//     double y = bottomLeft.y + (deltaY * COS_60 - deltaX * SIN_60);
//     GPoint tip(x, y);
//     return tip;
// }

// // Returns a GPoint that is a fraction of the way from p1 to p2.
// GPoint pointBetween(GPoint p1, GPoint p2, double fraction)
// {
//     double x = p1.x + (p2.x - p1.x) * fraction;
//     double y = p1.y + (p2.y - p1.y) * fraction;
//     return GPoint(x, y);
// }

// void drawSnowflake(GWindow& w, int level, GPoint start, GPoint end)
// {
//     if (level == 1)
//     {
//         // We used the following at one point to slow down the animation.
//         // The argument is number of milliseconds to pause the program
//         // before executing the next instruction.
//         //pause(2000);
//         w.drawLine(start, end);
//         return;
//     }

//     GPoint a = pointBetween(start, end, 1.0/3);
//     GPoint b = pointBetween(start, end, 2.0/3);
//     GPoint t = thirdEquilateralPoint(a, b);

//     drawSnowflake(w, level - 1, start, a);
//     drawSnowflake(w, level - 1, a, t);
//     drawSnowflake(w, level - 1, t, b);
//     drawSnowflake(w, level - 1, b, end);
// }

// Returns a GPoint that forms an equilateral triangle with the points passed as
// parameters.
GPoint thirdEquilateralPoint(GPoint bottomLeft, GPoint otherPoint) {
    double deltaX = otherPoint.x - bottomLeft.x;
    double deltaY = otherPoint.y - bottomLeft.y;
    double x = bottomLeft.x + deltaX * COS_60 + deltaY * SIN_60;
    double y = bottomLeft.y + deltaY * COS_60 - deltaX * SIN_60;
    return GPoint(x,y);
}

// Returns a GPoint that is a fraction of the way from p1 to p2.
GPoint pointBetween(GPoint p1, GPoint p2, double fraction) {
    double x = p1.x + (p2.x - p1.x) * fraction;
    double y = p1.y + (p2.y - p1.y) * fraction;
    return GPoint(x,y);
}

void drawSnowflake(GWindow& w, int level, GPoint start, GPoint end)
{
    // To draw a line from p1 to p2, the syntax is:
    //
    //    w.drawLine(p1, p2);

    if (level == 1) {
        w.drawLine(start, end);
        return;
    }
    GPoint a = pointBetween(start, end, 1.0 / 3);
    GPoint b = pointBetween(start, end, 2.0 / 3);
    GPoint t = thirdEquilateralPoint(a, b);
    drawSnowflake(w, level - 1, start, a);
    drawSnowflake(w, level - 1, a, t);
    drawSnowflake(w, level - 1, t, b);
    drawSnowflake(w, level - 1, b, end);
}

int main_snowflake()
{
    // This creates the graphics window.
    GWindow w(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Determine the bottom-left and bottom-right points for the
    // snowflake based on constants defined at the top of the code.
    GPoint bottomLeft(BASE_LEFT_X, BASE_Y);
    GPoint bottomRight(BASE_RIGHT_X, BASE_Y);

    w.setColor("black");
    w.setFillColor("black");

    drawSnowflake(w, 5, bottomLeft, bottomRight);

    return 0;
}
