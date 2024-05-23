#include "Plotter.h"
#include "strlib.h"
#include "vector.h"
#include "error.h"
using namespace std;

void runPlotterScript(istream& input) {
    PenStyle penstyle = {1.0, "black"};
    double x1 = 0.0, y1 = 0.0;
    double x2, y2;
    bool isPenDown = false;
    for (string line; getline(input, line); ) {
        /* ... do something with line ... */
        Vector<string> tokens = stringSplit(line, " ");
        string command = toLowerCase(tokens[0]);
        if (command == "pendown") {
            isPenDown = true;
        } else if (command == "penup") {
            isPenDown = false;
        } else if (command == "moveabs") {
            x2 = stringToDouble(tokens[1]);
            y2 = stringToDouble(tokens[2]);
            if (isPenDown) {
                drawLine(x1, y1, x2, y2, penstyle);
            }
            x1 = x2;
            y1 = y2;
        } else if (command == "moverel") {
            x2 = x1 + stringToDouble(tokens[1]);
            y2 = y1 + stringToDouble(tokens[2]);
            if (isPenDown) {
                drawLine(x1, y1, x2, y2, penstyle);
            }
            x1 = x2;
            y1 = y2;
        } else if (command == "pencolor") {
            penstyle.color = tokens[1];
        } else if (command == "penwidth") {
            penstyle.width = stringToDouble(tokens[1]);
        } else {
            error("there are commands can't be compiled in the file");
        }
    }
}
