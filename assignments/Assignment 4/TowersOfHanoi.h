/*
 * File: HanoiGraphics.h
 * --------------
 * This file defines the interface for the graphical Hanoi functions.
 */

#ifndef HanoiGraphics_Included
#define HanoiGraphics_Included

#include "gwindow.h"

enum class AnimationSpeed {
    MOLASSES,
    CHEETAH,
    PEREGRINE_FALCON
};

/*
 * Function: initHanoiDisplay
 * Usage: initHanoiDisplay(5, AnimationSpeed::MOLASSES);
 * -------------------------------
 * Initializes the graphics window and draws the starting configuration
 * for a Towers of Hanoi puzzle for n disks. The speed is determined by
 * the final parameter.
 */

void initHanoiDisplay(int n, AnimationSpeed speed = AnimationSpeed::MOLASSES);

/*
 * Function: moveSingleDisk
 * Usage: moveSingleDisk(char start, char finish);
 * -----------------------------------------------
 * Moves a single disk from the start tower to the finish
 * tower.  This method generates an error if the start tower
 * is empty or if the move violates the rules of the Hanoi
 * puzzle by placing a larger disk on top of a smaller one.
 */

void moveSingleDisk(char start, char finish);

int solveTowersOfHanoi(int numDisks, char start, char finish, char temp);

#endif
