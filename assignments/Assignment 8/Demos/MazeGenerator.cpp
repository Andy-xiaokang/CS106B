#include "MazeGenerator.h"
#include <utility>
#include <random>
#include <vector>
#include <map>
#include "strlib.h"
#include "grid.h"
#include "hashset.h"
#include "queue.h"
#include "vector.h"
using namespace std;

namespace {
    /* Creates a blank grid maze of the given size. */
    Grid<MazeCell*> blankMaze(int numRows, int numCols) {
        Grid<MazeCell*> result(numRows, numCols);
        for (int row = 0; row < numRows; row++) {
            for (int col = 0; col < numCols; col++) {
                result[row][col] = new MazeCell();
            }
        }
        return result;
    }

    /* Type representing a possible edge in a maze. Two MazeCells are each
     * included here, along with the fields that would link them.
     */
    struct EdgeBuilder {
        /* Which two edges to link. */
        MazeCell* first;
        MazeCell* second;

        /* How to link them. */
        MazeCell* MazeCell::* firstField;
        MazeCell* MazeCell::* secondField;
    };

    /* Generates a random number in the specified range. We use this instead of
     * std::uniform_int_distribution because std::uniform_int_distribution is
     * not cross-platform compatible.
     */
    int mtRandomBetween(int low, int high, mt19937& generator) {
        return low + generator() % (high - low + 1);
    }

    /* Generates a random real number between 0 and 1. We use this instead of
     * std::uniform_real_distribution because std::uniform_real_distribution is
     * not cross-platform compatible.
     */
    double mtRandomProbability(mt19937& generator) {
        while (true) {
            uint32_t value = generator();
            uint32_t max   = numeric_limits<decltype(value)>::max();

            if (value != max) {
                return double(value) / max;
            }
        }
    }

    /* Returns all possible edges that could appear in a grid maze. */
    vector<EdgeBuilder> allPossibleEdgesFor(const Grid<MazeCell*>& maze) {
        vector<EdgeBuilder> result;
        for (int row = 0; row < maze.numRows(); row++) {
            for (int col = 0; col < maze.numCols(); col++) {
                if (row + 1 < maze.numRows()) {
                    result.push_back({ maze[row][col], maze[row + 1][col], &MazeCell::south, &MazeCell::north });
                }
                if (col + 1 < maze.numCols()) {
                    result.push_back({ maze[row][col], maze[row][col + 1], &MazeCell::east, &MazeCell::west });
                }
            }
        }
        return result;
    }

    /* Union-find FIND operation. */
    MazeCell* repFor(const map<MazeCell*, MazeCell*>& reps, MazeCell* cell) {
        while (reps.at(cell) != cell) {
            cell = reps.at(cell);
        }
        return cell;
    }

    /* Shuffles the edges using the Fischer-Yates shuffle. We provide our own
     * implementation because the std::shuffle implementations on Windows, Mac,
     * and Linux are not guaranteed to work in the same way.
     */
    void shuffleEdges(vector<EdgeBuilder>& edges, mt19937& generator) {
        for (size_t i = 0; i < edges.size(); i++) {
            size_t j = mtRandomBetween(i, edges.size() - 1, generator);
            swap(edges[i], edges[j]);
        }
    }

    /* Creates a random maze of the given size using a randomized Kruskal's
     * algorithm. Edges are shuffled and added back in one at a time, provided
     * that each insertion links two disconnected regions.
     */
    Grid<MazeCell*> makeMaze(int numRows, int numCols, mt19937& generator) {
        auto maze = blankMaze(numRows, numCols);

        auto edges = allPossibleEdgesFor(maze);
        shuffleEdges(edges, generator);

        /* Union-find structure, done without path compression because N is small. */
        map<MazeCell*, MazeCell*> representatives;
        for (auto* elem: maze) {
            representatives[elem] = elem;
        }

        /* Run a randomized Kruskal's algorithm to build the maze. */
        int edgesLeft = numRows * numCols - 1;
        for (size_t i = 0; edgesLeft > 0 && i < edges.size(); i++) {
            auto edge = edges[i];

            /* See if they're linked already. */
            auto* rep1 = repFor(representatives, edge.first);
            auto* rep2 = repFor(representatives, edge.second);

            /* If not, link them. */
            if (rep1 != rep2) {
                representatives[rep1] = rep2;

                edge.first->*edge.firstField   = edge.second;
                edge.second->*edge.secondField = edge.first;

                edgesLeft--;
            }
        }
        if (edgesLeft != 0) error("Unable to build maze.");

        return maze;
    }

    /* Clears all the links between the given group of nodes. */
    void clearGraph(Vector<MazeCell*>& nodes) {
        for (auto* node: nodes) {
            *node = MazeCell();
        }
    }

    /* Returns a random unassigned link from the given node, or nullptr if
     * they are all assigned.
     */
    MazeCell* MazeCell::* randomFreePortOf(MazeCell* cell, mt19937& generator) {
        Vector<MazeCell* MazeCell::*> ports;
        if (cell->east  == nullptr) ports += &MazeCell::east;
        if (cell->west  == nullptr) ports += &MazeCell::west;
        if (cell->north == nullptr) ports += &MazeCell::north;
        if (cell->south == nullptr) ports += &MazeCell::south;
        if (ports.isEmpty()) return nullptr;

        int port = mtRandomBetween(0, ports.size() - 1, generator);
        return ports[port];
    }

    /* Use a variation of the Erdos-Renyi random graph model. We set the
     * probability of any pair of nodes being connected to be ln(n) / n,
     * then artificially constrain the graph so that no node has degree
     * four or more. We generate mazes this way until we find one that's
     * conencted.
     */
    bool erdosRenyiLink(Vector<MazeCell*>& nodes, mt19937& generator) {
        /* High probability that everything is connected. */
        double threshold = log(nodes.size()) / nodes.size();

        for (int i = 0; i < nodes.size(); i++) {
            for (int j = i + 1; j < nodes.size(); j++) {
                if (mtRandomProbability(generator) <= threshold) {
                    auto iLink = randomFreePortOf(nodes[i], generator);
                    auto jLink = randomFreePortOf(nodes[j], generator);

                    /* Oops, no free links. */
                    if (iLink == nullptr || jLink == nullptr) {
                        return false;
                    }

                    nodes[i]->*iLink = nodes[j];
                    nodes[j]->*jLink = nodes[i];
                }
            }
        }

        return true;
    }

    /* Returns whether the given maze is fully connected. */
    bool isConnected(const Vector<MazeCell*>& maze) {
        HashSet<MazeCell*> visited;
        Queue<MazeCell*> frontier;
        frontier.enqueue(maze[0]);

        while (!frontier.isEmpty()) {
            auto* curr = frontier.dequeue();

            if (!visited.contains(curr)) {
                visited += curr;

                if (curr->east  != nullptr) frontier.enqueue(curr->east);
                if (curr->west  != nullptr) frontier.enqueue(curr->west);
                if (curr->north != nullptr) frontier.enqueue(curr->north);
                if (curr->south != nullptr) frontier.enqueue(curr->south);
            }
        }

        return visited.size() == maze.size();
    }

    /* Generates a random twisty maze. This works by repeatedly generating
     * random graphs until a connected one is found.
     */
    Vector<MazeCell*> makeTwistyMaze(int numNodes, mt19937& generator) {
        Vector<MazeCell*> result;
        for (int i = 0; i < numNodes; i++) {
            result += new MazeCell();
        }

        /* Keep generating mazes until we get a connected one. */
        do {
            clearGraph(result);
        } while (!erdosRenyiLink(result, generator) || !isConnected(result));

        return result;
    }

    /* Returns if two nodes are adjacent. */
    bool areAdjacent(MazeCell* first, MazeCell* second) {
        return first->east  == second ||
               first->west  == second ||
               first->north == second ||
               first->south == second;
    }

    /* Uses the Floyd-Warshall algorithm to compute the shortest paths between all
     * pairs of nodes in the maze. The result is a table where table[i][j] is the
     * shortest path distance between maze[i] and maze[j].
     */
    Grid<int> allPairsShortestPaths(const Vector<MazeCell*>& maze) {
        /* Floyd-Warshall algorithm. Fill the grid with "infinity" values. */
        Grid<int> result(maze.size(), maze.size(), maze.size() + 1);

        /* Set distances of nodes to themselves at 0. */
        for (int i = 0; i < maze.size(); i++) {
            result[i][i] = 0;
        }

        /* Set distances of edges to 1. */
        for (int i = 0; i < maze.size(); i++) {
            for (int j = 0; j < maze.size(); j++) {
                if (areAdjacent(maze[i], maze[j])) {
                    result[i][j] = 1;
                }
            }
        }

        /* Dynamic programming step. Keep expanding paths by allowing for paths
         * between nodes.
         */
        for (int i = 0; i < maze.size(); i++) {
            Grid<int> next(result.numRows(), result.numCols());
            for (int j = 0; j < maze.size(); j++) {
                for (int k = 0; k < maze.size(); k++) {
                    next[j][k] = min(result[j][k], result[j][i] + result[i][k]);
                }
            }
            result = next;
        }

        return result;
    }

    /* Given a list of distinct nodes, returns the "score" for their distances,
     * which is a sequence of numbers representing pairwise distances in sorted
     * order.
     */
    Vector<int> scoreOf(const Vector<int>& nodes, const Grid<int>& distances) {
        Vector<int> result;

        for (int i = 0; i < nodes.size(); i++) {
            for (int j = i + 1; j < nodes.size(); j++) {
                result += distances[nodes[i]][nodes[j]];
            }
        }

        sort(result.begin(), result.end());
        return result;
    }

    /* Given a grid, returns a combination of four nodes whose overall score
     * (sorted list of pairwise distances) is as large as possible in a
     * lexicographical sense.
     */
    Vector<int> remoteLocationsIn(const Grid<int>& distances) {
        Vector<int> result = {0, 1, 2, 3};

        /* We could do this recursively, but since it's "only" four loops
         * we'll just do that instead. :-)
         */
        for (int i = 0; i < distances.numRows(); i++) {
            for (int j = i + 1; j < distances.numRows(); j++) {
                for (int k = j + 1; k < distances.numRows(); k++) {
                    for (int l = k + 1; l < distances.numRows(); l++) {
                        Vector<int> curr = { i, j, k, l };
                        if (scoreOf(curr, distances) > scoreOf(result, distances)) {
                            result = curr;
                        }
                    }
                }
            }
        }

        return result;
    }

    /* Copies items from the text maze over to the grid maze. */
    void fillItems(Grid<MazeCell*>& maze, const Vector<string>& textMaze) {
        for (int row = 0; row < maze.numRows(); row++) {
            for (int col = 0; col < maze.numCols(); col++) {
                /* Odd-numbered positions are interstitial positions. */
                switch (textMaze[2 * row][2 * col]) {
                case 'S':
                    maze[row][col]->whatsHere = Item::SPELLBOOK;
                    break;
                case 'P':
                    maze[row][col]->whatsHere = Item::POTION;
                    break;
                case 'W':
                    maze[row][col]->whatsHere = Item::WAND;
                    break;
                case '*':
                    maze[row][col]->whatsHere = Item::NOTHING;
                    break;
                default:
                    error("Unknown character found when copying items.");
                }
            }
        }
    }

    /* Fills in the walls in maze given a text pattern. */
    void addWalls(Grid<MazeCell*>& maze, const Vector<string>& textMaze) {
        for (int row = 0; row < maze.numRows(); row++) {
            for (int col = 0; col < maze.numCols(); col++) {
                /* Look down if that's possible. */
                if (row + 1 < maze.numRows() && textMaze[2 * row + 1][2 * col] == '|') {
                    maze[row][col]->south = maze[row + 1][col];
                    maze[row + 1][col]->north = maze[row][col];
                }
                /* Look left if that's possible. */
                if (col + 1 < maze.numCols() && textMaze[2 * row][2 * col + 1] == '-') {
                    maze[row][col]->east = maze[row][col + 1];
                    maze[row][col + 1]->west = maze[row][col];
                }
            }
        }
    }

    /* Validates that a text maze is structured correctly. */
    void validateMaze(const Vector<string>& textMaze) {
        /* Number of rows should be odd since every other space is a location.
         * Same with the number of columns.
         */
        if (textMaze.size() % 2 != 1) {
            error("Maze should have an odd number of rows.");
        }
        if (textMaze[0].size() % 2 != 1) {
            error("Maze should have an odd number of columns.");
        }

        int numRows = 1 + textMaze.size() / 2;
        int numCols = 1 + textMaze[0].size() / 2;

        for (int row = 0; row < numRows; row++) {
            if (textMaze[row].size() != textMaze[0].size()) {
                error("Not all rows in the maze have the same size.");
            }
        }

        /* Confirm that there are blank spaces at all odd/odd pairs. */
        for (int row = 0; row + 1 < numRows; row++) {
            for (int col = 0; col + 1 < numCols; col++) {
                if (textMaze[2 * row + 1][2 * col + 1] != ' ') {
                    error("Found non-blank character where blank was expected.");
                }
            }
        }

        /* Confirm each location is either *, S, P, or W. */
        for (int row = 0; row < numRows; row++) {
            for (int col = 0; col < numCols; col++) {
                switch (textMaze[2 * row][2 * col]) {
                case 'P': case 'S': case 'W': case '*':
                    /* Do nothing. */
                    break;
                default:
                    error("Unknown character found at lattice point.");
                }
            }
        }

        /* Confirm each wall is either -, |, or blank. */
        for (int row = 0; row < numRows; row++) {
            for (int col = 0; col < numCols; col++) {
                if (row + 1 < numRows &&
                    textMaze[2 * row + 1][2 * col] != ' ' &&
                    textMaze[2 * row + 1][2 * col] != '|') {
                    error("Unknown wall character.");
                }
                if (col + 1 < numCols &&
                    textMaze[2 * row][2 * col + 1] != ' ' &&
                    textMaze[2 * row][2 * col + 1] != '-') {
                    error("Unknown wall character.");
                }
            }
        }
    }
}

const int kNumRows = 4;
const int kNumCols = 4;

MazeCell* mazeFor(const string& name) {
    /* Seed a generator using the provided seed. From this point forward, only
     * use random numbers from this source.
     */
    mt19937 generator(hashCode(name, kNumRows, kNumCols));
    auto maze = makeMaze(kNumRows, kNumCols, generator);

    /* Place the items and start locations far away from one another. */
    Vector<MazeCell*> linearMaze;
    for (MazeCell* cell: maze) {
        linearMaze += cell;
    }
    auto distances = allPairsShortestPaths(linearMaze);
    auto locations = remoteLocationsIn(distances);

    /* Place the items. */
    linearMaze[locations[1]]->whatsHere = Item::SPELLBOOK;
    linearMaze[locations[2]]->whatsHere = Item::POTION;
    linearMaze[locations[3]]->whatsHere = Item::WAND;

    /* We begin in position 0. */
    return linearMaze[locations[0]];
}

const int kNumTwistyRooms = 12;

MazeCell* twistyMazeFor(const string& name) {
    /* Seed a generator using the provided seed. From this point forward, only
     * use random numbers from this source.
     */
    mt19937 generator(hashCode(name, kNumTwistyRooms));
    auto maze = makeTwistyMaze(kNumTwistyRooms, generator);

    /* Find the distances between all pairs of nodes. */
    auto distances = allPairsShortestPaths(maze);

    /* Select a 4-tuple maximizing the minimum distances between points,
     * and use that as our item/start locations.
     */
    auto locations = remoteLocationsIn(distances);

    /* Place the items there. */
    maze[locations[1]]->whatsHere = Item::SPELLBOOK;
    maze[locations[2]]->whatsHere = Item::POTION;
    maze[locations[3]]->whatsHere = Item::WAND;

    return maze[locations[0]];
}

/* Converts a text representation of a maze into a Grid<MazeCell*> objects. */
Grid<MazeCell*> toMaze(const Vector<string>& textMaze) {
    validateMaze(textMaze);

    int numRows = 1 + textMaze.size() / 2;
    int numCols = 1 + textMaze[0].size() / 2;

    auto result = blankMaze(numRows, numCols);
    fillItems(result, textMaze);
    addWalls(result, textMaze);
    return result;
}
