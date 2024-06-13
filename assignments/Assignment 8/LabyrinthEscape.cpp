#include "Demos/MazeGenerator.h"
#include "GUI/SimpleTest.h"
using namespace std;

/* Change this constant to contain your name (and your partner's name, if you're
 * working in a pair).
 *
 * WARNING: Once you've set set this constant and started exploring your maze,
 * do NOT edit the value of MyName. Changing MyName will change which maze you
 * get back, which might invalidate all your hard work!
 *
 * You might be wondering - why are we using the weird type "const char MyName[]"
 * rather than "const string MyName?" While either is fine, the version we've used
 * here makes a raw array of characters containing your name and thus eliminates
 * a compiler warning message. Come talk to us over EdStem if you'd like to learn
 * more about this!
 */
const char MyName[] = "kang";

/* Change these constants to contain the paths out of your mazes. */
const char ThePathOutOfMyMaze[] = "SENESESNWNWSSWSEWNEESE";
const char ThePathOutOfMyTwistyMaze[] = "NSESNNEEWWNNW";

PROVIDED_TEST("Escape from the labyrinth!") {
    /* A maze for you to escape from. This maze will be personalized
     * based on the constant MyName.
     *
     * Do not set a breakpoint on this line. Instead, set it before the
     * EXPECT statement below.
     */
    MazeCell* startLocation = mazeFor(MyName);

    /* Set a breakpoint on the next line. As a reminder, the labyrinth you
     * get will be personalized to you, so don't start exploring the labyrinth
     * unless you've edited the constant MyName to include your name(s)!
     * Otherwise, you'll be escaping the wrong labyrinth.
     *
     * This is the labyrinth you'll escape from in Milestone Two. The maze
     * will be in the shape of a regular grid, with pointers pointing in the
     * directions you expect them to point.
     */
    EXPECT(isPathToFreedom(startLocation, ThePathOutOfMyMaze));

    /* We are aware that we didn't free any of the memory allocated in this maze,
     * causing a memory leak. Since this is purely for educational purposes,
     * we're going to let that slide, but you should generally not write code
     * like this.
     */
}





PROVIDED_TEST("Escape from the twisty labyrinth!") {
    /* Do not set a breakpoint here; set it at the EXPECT statement. */
    MazeCell* startLocation = twistyMazeFor(MyName);

    /* This test case is for Milestone 3.
     *
     *  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     *  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     *  >>>> Do not start working on this section until you've read the <<<<
     *  >>>> instructions for Milestone 3, the one that talks about     <<<<
     *  >>>> twisty mazes (the ones where the maze is not a regular     <<<<
     *  >>>> grid of cells. The maze here is structured differently     <<<<
     *  >>>> the maze in Milestone 2, and escaping it requires a        <<<<
     *  >>>> different set of skills.                                   <<<<
     *  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     *  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
     *
     * This is the labyrinth you'll escape from in Milestone Three. The
     * maze is not necessarily a rectangular grid, and the pointers in each
     * maze cell might not point in the direction they're labeled as pointing.
     * However, you can rely on the fact that if one MazeCell links to a
     * second, the second (somehow) links directly back to the first.
     */
    EXPECT(isPathToFreedom(startLocation, ThePathOutOfMyTwistyMaze));

    /* We are aware that we didn't free any of the memory allocated in this maze,
     * causing a memory leak. Since this is purely for educational purposes,
     * we're going to let that slide, but you should generally not write code
     * like this.
     */
}
