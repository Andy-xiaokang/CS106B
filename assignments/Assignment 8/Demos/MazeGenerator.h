#ifndef MazeGenerator_Included
#define MazeGenerator_Included

#include "../Labyrinth.h"
#include "grid.h"
#include "vector.h"
#include <string>

/**
 * Given a graphical representation of a maze, returns a Grid<MazeCell*>
 * contining the contents of that maze.
 *
 * For example, here's the input you'd give to represent the first example
 * maze from the handout:
 *
 *         {"* *-W *",
 *          "| |   |",
 *          "*-* * *",
 *          "  | | |",
 *          "S *-*-*",
 *          "|   | |",
 *          "*-*-* P"}
 *
 * Here, stars indicate empty spots and the remaining letters indicate where
 * the Wand, Spellbook, and Potion are. (Since we're handing the whole maze
 * back to you, you can choose where you want to begin.) The | and - symbols
 * are used to indicate which cells link to which other cells.
 *
 * Mazes can have any size and any number of copies of the items, even though
 * a "real" maze would only have one of each item.
 */
Grid<MazeCell*> toMaze(const Vector<std::string>& textMaze);

/**
 * Returns a maze of size numRows x numCols, specifically tailored to
 * the given name.
 *
 * We've implemented this function for you. You don't need to write it
 * yourself.
 *
 * Please don't make any changes to this function - we'll be using our
 * reference version when testing your code, and it would be a shame if
 * the maze you solved wasn't the maze we wanted you to solve!
 */
MazeCell* mazeFor(const std::string& name);

/**
 * Returns a Twisty Maze with the specified number of nodes.
 *
 * Please don't make any changes to this function - we'll be using our
 * reference version when testing your code, and it would be a shame if
 * the maze you solved wasn't the maze we wanted you to solve!
 */
MazeCell* twistyMazeFor(const std::string& name);

#endif
