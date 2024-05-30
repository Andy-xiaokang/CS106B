#include <iostream>
#include "TowersOfHanoi.h"
using namespace std;

/* Constant: kPauseTime
 *
 * Amount of time to pause for when beginning the Towers Of Hanoi animation.
 */
const double kPauseTime = 500;

/* This function is adapted from the one in Chapter 8.1 of the textbook. If you
 * haven't yet read that chapter, I recommend that you do so before starting
 * this problem.
 */
void moveTower(int numDisks, char start, char finish, char temp, int& totalMoves) {
    if (numDisks != 0) {
        moveTower(numDisks - 1, start, temp, finish, totalMoves);
        moveSingleDisk(start, finish);
        totalMoves++;
        moveTower(numDisks - 1, temp, finish, start, totalMoves);
    }
}

int solveTowersOfHanoi(int numDisks, char start, char finish, char temp) {
    /* Want to slow this down? Try these other options for the final parameter:
     *
     *   AnimationSpeed::MOLASSES         (extremely slow)
     *   AnimationSpeed::CHEETAH          (regular)
     *   AnimationSpeed::PEREGRINE_FALCON (extremely fast)
     */
    initHanoiDisplay(numDisks, AnimationSpeed::CHEETAH);
    pause(kPauseTime);

    int totalMoves = 0;
    moveTower(numDisks, start, finish, temp, totalMoves);
    return totalMoves;
}
