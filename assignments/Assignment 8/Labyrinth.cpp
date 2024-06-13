#include "Labyrinth.h"
using namespace std;

struct flag {
    bool flag1;
    bool flag2;
    bool flag3;
    flag(bool visited1, bool visited2, bool visited3) {
        flag1 = visited1;
        flag2 = visited2;
        flag3 = visited3;
    }
};

bool helper(MazeCell* start, string moves, flag& visited) {
    if (start->whatsHere == Item::SPELLBOOK) visited.flag1 = true;
    if (start->whatsHere == Item::WAND) visited.flag2 = true;
    if (start->whatsHere == Item::POTION) visited.flag3 = true;
    if (visited.flag1 == true && visited.flag2 == true && visited.flag3 == true) return true;
    if (moves.size() == 0) return false;
    string move = moves.substr(0, 1);
    moves = moves.substr(1);
    if (move == "N") {
        if (start->north) {
            start = start->north;
            return helper(start, moves, visited);
        }
        return false;
    } else if (move == "S") {
        if (start->south) {
            start = start->south;
            return helper(start, moves, visited);
        }
        return false;
    } else if (move == "W") {
        if (start->west) {
            start = start->west;
            return helper(start, moves, visited);
        }
        return false;
    } else if (move == "E") {
        if (start->east) {
            start = start->east;
            return helper(start, moves, visited);
        }
        return false;
    }
    return false;
}

bool isPathToFreedom(MazeCell* start, const string& moves) {
    flag visited(false, false, false);
    return helper(start, moves, visited);
}


/* * * * * * Test Cases Below This Point * * * * * */
#include "GUI/SimpleTest.h"
#include "Demos/MazeGenerator.h"

/* Optional: Add your own custom tests here! */














/* * * * * Provided Tests Below This Point * * * * */

/* Utility function to free all memory allocated for a maze. */
void deleteMaze(const Grid<MazeCell*>& maze) {
    for (auto* elem: maze) {
        delete elem;
    }
    /* Good question to ponder: why don't we write 'delete maze;'
     * rather than what's shown above?
     */
}

PROVIDED_TEST("Checks paths in the sample maze.") {
    auto maze = toMaze({"* *-W *",
                        "| |   |",
                        "*-* * *",
                        "  | | |",
                        "S *-*-*",
                        "|   | |",
                        "*-*-* P"});

    /* These paths are the ones in the handout. They all work. */
    EXPECT(isPathToFreedom(maze[2][2], "ESNWWNNEWSSESWWN"));
    EXPECT(isPathToFreedom(maze[2][2], "SWWNSEENWNNEWSSEES"));
    EXPECT(isPathToFreedom(maze[2][2], "WNNEWSSESWWNSEENES"));

    /* These paths don't work, since they don't pick up all items. */
    EXPECT(!isPathToFreedom(maze[2][2], "ESNW"));
    EXPECT(!isPathToFreedom(maze[2][2], "SWWN"));
    EXPECT(!isPathToFreedom(maze[2][2], "WNNE"));

    /* These paths don't work, since they aren't legal paths. */
    EXPECT(!isPathToFreedom(maze[2][2], "WW"));
    EXPECT(!isPathToFreedom(maze[2][2], "NN"));
    EXPECT(!isPathToFreedom(maze[2][2], "EE"));
    EXPECT(!isPathToFreedom(maze[2][2], "SS"));

    deleteMaze(maze);
}

PROVIDED_TEST("Can't walk through walls.") {
    auto maze = toMaze({"* S *",
                        "     ",
                        "W * P",
                        "     ",
                        "* * *"});

    EXPECT(!isPathToFreedom(maze[1][1], "WNEES"));
    EXPECT(!isPathToFreedom(maze[1][1], "NWSEE"));
    EXPECT(!isPathToFreedom(maze[1][1], "ENWWS"));
    EXPECT(!isPathToFreedom(maze[1][1], "SWNNEES"));

    deleteMaze(maze);
}

PROVIDED_TEST("Works when starting on an item.") {
    auto maze = toMaze({"P-S-W"});

    EXPECT(isPathToFreedom(maze[0][0], "EE"));
    EXPECT(isPathToFreedom(maze[0][1], "WEE"));
    EXPECT(isPathToFreedom(maze[0][2], "WW"));

    deleteMaze(maze);
}

