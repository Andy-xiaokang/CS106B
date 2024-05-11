/*
 * CS106B Section Handout Test Harness: Section 2
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified for Qt and Ed by Neel
 * Kishnani for Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include "testing/SimpleTest.h"
#include "map.h"
#include <string>
using namespace std;

bool areAtSameCompany(const string& p1,
                      const string& p2,
                      const Map<string, string>& bosses) {
    string head1 = p1, head2 = p2;
    while (bosses.containsKey(head1)) {
        head1 = bosses[head1];
    }
    while (bosses.containsKey(head2)) {
        head2 = bosses[head2];
    }
    if (head1 == head2) {
        return true;
    } else {
        return false;
    }
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Basic tests for areAtSameCompany") {
    Map<string, string> company1 = {
        { "Neel", "Keith" },
        { "Keith", "Mehran" },
        { "Mehran", "John" }, 
        { "John", "Jennifer" }, 
        { "Jennifer", "Persis" }
    };

    Map<string, string> company2 = {
        { "CJ", "Pierre" },
        { "Jack", "Stanley" },
        { "Stanley", "Lisa" },
        { "Lisa", "Janet" },
        { "Janet", "Poonam" }

    };

    EXPECT(areAtSameCompany("Neel", "Jennifer", company1));
    EXPECT(!areAtSameCompany("Neel", "Lisa", company1));
    EXPECT(!areAtSameCompany("CJ", "Keith", company2));
    EXPECT(!areAtSameCompany("Poonam", "Lisa", company1));

}
