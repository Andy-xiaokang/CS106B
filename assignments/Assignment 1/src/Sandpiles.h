/******************************************************************************
 * File: Sandpiles.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Header file for the Sandpiles portion of the assignment. You should not
 * modify this file.
 */

#ifndef Sandpile_Included
#define Sandpile_Included

#include "grid.h"

/**
 * Drops a grain of sand into the specific world at the specified position.
 *
 * If the position is out of bounds, then this function has no effect and the grid
 * is left unmodified.
 *
 * This may trigger a topple, which may propagate to other locations in the grid. A
 * cell topples when it reaches four grains, and when it topples, it's emptied out
 * and a grain of sand is dropped on each of the four neighbors.
 *
 * @param world The grid into which to drop the sand.
 * @param row The row into which the sand is dropped.
 * @param col The column into which the sand is dropped.
 */
void dropSandOn(Grid<int>& world, int row, int col);

#endif
