/*
 * CS106B Section Handout Test Harness: Section 1
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified by Trip Master and Nick
 * Bowman for CS106B Spring 2020.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include <iostream>
#include"console.h"
#include "map.h"
#include "vector.h"
#include "filelib.h"
#include "strlib.h"
#include "set.h"
#include "SimpleTest.h"
using namespace std;

/*
 * Friend List (Code Write)
 * ----------------------------------
 * Write a function named friendList that takes in a file name and reads
 * friend relationships from a file and writes them to a Map.
 * friendList should return the populated Map. Friendships are bi-directional.
 * The file contains one friend relationship per line, with names separated
 * by a single space. You do not have to worry about malformed entries.
 */

Map<string, Set<string>> friendList(string filename) {
    Map<string, Set<string>> map;
    ifstream ifs;
    if (!openFile(ifs,filename)) {
        cout << "failed to open file" << filename << endl;
        return {};
    }
    Vector<string> lines = readLines(ifs);
    for (string line : lines) {
        Vector<string> names = stringSplit(line, ' ');
        map[names[0]].add(names[1]);
        map[names[1]].add(names[0]);
    }
    return map;
}


Set<string> mutualList(Map<string, Set<string> >& friendList, string friend1, string friend2) {
    return (friendList[friend1] * friendList[friend2]);
}
///* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("provided test case from handout for friendList") {
    Map<string, Set<string>> soln;
    soln["Abby"] = {"Barney", "Clyde"};
    soln["Barney"] = {"Abby"};
    soln["Clyde"] = {"Abby"};
    EXPECT_EQUAL(soln, friendList("res/buddies.txt"));
}

PROVIDED_TEST("provided test case from handout for mutualList") {
    Map<string, Set<string>> friendList;
    friendList["Abby"] = {"Barney", "Clyde"};
    friendList["Barney"] = {"Abby"};
    friendList["Clyde"] = {"Abby"};
    EXPECT_EQUAL(mutualList(friendList, "Barney", "Clyde"), {"Abby"});
    EXPECT_EQUAL(mutualList(friendList, "Barney", "Abby"), {});
}
