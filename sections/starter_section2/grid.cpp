/*
 * CS106B Section Handout Test Harness: Section 2
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified by Trip Master and Nick
 * Bowman for CS106B Spring 2020.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include "SimpleTest.h"
#include "grid.h"
#include "gridlocation.h"

using namespace std;

/*
 * Max row (Code Write)
 * ----------------------------------
 * Write a function named maxRow that takes in a grid and an in 
 * bounds gridLocation and returns a the maximum value in that 
 * row of the grid.
 */

int maxRow(Grid<int>& grid, GridLocation loc) {
    int row = loc.row;
    int result = 0;
    for (int j = 0; j < grid.numCols(); j++) {
        if (result < grid[row][j]) {
            result = grid[row][j];
        }
    }
    return result;
}

/*
 * Avg neighborhood (Code Write)
 * ----------------------------------
 * Write a function named avgNeighborhood that takes in a grid and an in 
 * bounds gridLocation and returns the average value in the neighborhood
 * of the gridLocation. A neighborhood is defined as the in bounds 
 * locations in North, South, East and West of loc.
 */
int avgNeighborhood(Grid<int>& grid, GridLocation loc) {
    int row = grid.numRows();
    int col = grid.numCols();
    int x = loc.row;
    int y = loc.col;

    Grid<int> newGrid = Grid(row + 2, col + 2, 0);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            newGrid[i+1][j+1] = grid[i][j];
        }
    }
    cout << newGrid << endl;
    int nums = 0;
    if (( x == 0 && y == 0) || (x == 0 && y == col - 1) || (x == row - 1 && y == 0) || (x == row - 1 && y == col - 1)) {
        nums = 2;
    } else if ((x == 0) || (x == row - 1) || (y == 0) || (y == col - 1)) {
        nums = 3;
    } else {
        nums = 4;
    }
    int newx = x + 1;
    int newy = y + 1;
    cout << "rows: " << row << endl;
    cout << "cols: " << col << endl;
    cout << "nums: " << nums << endl;
    cout << "x: " << x << endl;
    cout << "y: " << y << endl;
    cout << (newGrid[newx -1][newy] + newGrid[newx + 1][newy] + newGrid[newx][newy - 1] + newGrid[newx][newy + 1]) << endl;
    cout << endl;
    return (newGrid[newx -1][newy] + newGrid[newx + 1][newy] + newGrid[newx][newy - 1] + newGrid[newx][newy + 1]) / nums;
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Test max row") {
    Grid<int> grid = {{0, 9, 1}, {1, 3, 0}, {2, 3, 1}};
    EXPECT_EQUAL(maxRow(grid, {2, 1}), 3);
    EXPECT_EQUAL(maxRow(grid, {0, 1}), 9);
}

PROVIDED_TEST("Test average neighborhood") {
    Grid<int> grid = {{0, 9, 1}, {1, 3, 0}, {2, 3, 1}};
    EXPECT_EQUAL(avgNeighborhood(grid, {0,0}), 5);
    // average is 3.33*, but we truncate to 3
    EXPECT_EQUAL(avgNeighborhood(grid, {1,1}), 3); 
}
