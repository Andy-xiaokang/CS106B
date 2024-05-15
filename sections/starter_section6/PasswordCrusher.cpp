/*
 * CS106B Section Handout Test Harness: Section 6
 * ----------------------------------------------
 * These problems have been galvanized from years of
 * section handouts that have been worked on by numerous
 * instructors and TA's. Codified for Qt and Ed by Neel
 * Kishnani for Winter 2022.
 *
 * A huge thank you to Keith Schwarz and Julie Zelenski
 * for creating an amazing testing harness!
 */

#include <iostream>
#include "testing/SimpleTest.h"
#include "testing/TextUtils.h"
#include "map.h"
#include "set.h"
#include "HashFunction.h"
#include "filelib.h"
#include "vector.h"
#include "random.h"
using namespace std;

Map<string, string>
breakWeakPasswords(const Map<string, int>& stolenPasswordFile,
                   const Set<string>& knownWeakPasswords,
                   HashFunction<string> hashFn) {
    Map<int, string> weakHashToPassword;
    for (string password : knownWeakPasswords) {
        weakHashToPassword[hashFn(password)] = password;
    }
    Map<string, string> result;
    for (string username : stolenPasswordFile) {
        if (weakHashToPassword.containsKey(hashFn(username))) {
            result[username] = weakHashToPassword[hashFn(username)];
        }
    }
    return result;
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Small test.") {
    // These are just set-ups.
    HashFunction<string> hashFn(10000, 0);
    ifstream s;
    Vector<string> passwords_vec = {};
    if (openFile(s, "res/commonPasswords.txt")) {
        readEntireFile(s, passwords_vec);
    } else {
        error("File reading failed...");
    }
    Set<string> passwords = {};
    for (const string& password: passwords_vec) {
        passwords.add(password);
    }
    // There should be no duplicate passwords.
    EXPECT_EQUAL(passwords.size(), 10000);

    // Test 1: Every password got leaked!
    Map<string, int> stolenPasswordFile1 = {};
    for (const string& password: passwords) {
        stolenPasswordFile1[password] = hashFn(password);
    }
    EXPECT_EQUAL(breakWeakPasswords(stolenPasswordFile1, passwords, hashFn).size(), 10000);

    // Test 2: Some random passwords got leaked!
    Map<string, int> stolenPasswordFile2 = {};
    int numLeaked = 0;
    for (const string& password: passwords) {
        if (randomChance(0.1)) {
            stolenPasswordFile2[password] = hashFn(password);
            numLeaked++;
        }
    }
    EXPECT_EQUAL(breakWeakPasswords(stolenPasswordFile2, passwords, hashFn).size(), numLeaked);
}
