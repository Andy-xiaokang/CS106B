/*
 * CS106B Section Handout Test Harness: Section 4
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
#include <string>
#include "testing/SimpleTest.h"
#include "testing/TextUtils.h"
#include "set.h"
#include "strlib.h"
using namespace std;

/*
 * Function: isElementSpellable
 * -----------------------------
 * Write a function that accepts as input a string and a Set<string>
 * containing all element symbols (stored with the proper
 * capitalization), then returns whether that string can be
 * written using only element symbols.
 */
bool startsWithIgnoreCase(const string& text, const string& symbol) {
    return startsWith(toLowerCase(text), toLowerCase(symbol));
}

bool isElementSpellable(const string& text, const Set<string>& symbols) {
    // base case
    if (text.empty()) {
        return true;
    }
    // recursive case
    for (string symbol : symbols) {
        if (startsWithIgnoreCase(text, symbol) && isElementSpellable(text.substr(symbol.size()), symbols)) {
                return true;
            }
    }
    return false;
}

bool isElementSpellable(const string& text, const Set<string>& symbols, string& result) {
    if (text.empty()) {
        cout << result << endl;
        return true;
    }
    for (string symbol : symbols) {
        if (startsWithIgnoreCase(text,symbol)) {
            if (isElementSpellable(text.substr(symbol.size()), symbols, result)) {
                result = symbol + result;
                return true;
            }
        }
    }
    return false;
}

/* * * * * Provided Tests Below This Point * * * * */

PROVIDED_TEST("Provided Test: Verify that the solution works on examples from the handout.") {
  Set<string> symbols = {
    "H", "He", "Li", "Be", "B",
    "C", "N", "O", "F", "Ne", "Na",
    "Mg", "Al", "Si", "P", "S", "Cl",
    "Ar", "K", "Ca", "Sc", "Ti", "V",
    "Cr", "Mn", "Fe", "Co", "Ni",
    "Cu", "Zn", "Ga", "Ge", "As",
    "Se", "Br", "Kr", "Rb", "Sr",
    "Y", "Zr", "Nb", "Mo", "Tc",
    "Ru", "Rh", "Pd", "Ag", "Cd",
    "In", "Sn", "Sb", "Te", "I",
    "Xe", "Cs", "Ba", "La", "Ce",
    "Pr", "Nd", "Pm", "Sm", "Eu",
    "Gd", "Tb", "Dy", "Ho", "Er",
    "Tm", "Yb", "Lu", "Hf", "Ta",
    "W", "Re", "Os", "Ir", "Pt",
    "Au", "Hg", "Tl", "Pb",
    "Bi", "Po", "At", "Rn", "Fr",
    "Ra", "Ac", "Th", "Pa", "U",
    "Np", "Pu", "Am", "Cm", "Bk",
    "Cf", "Es", "Fm", "Md", "No",
    "Lr", "Rf", "Db", "Sg", "Bh",
    "Hs", "Mt", "Ds", "Rg", "Cn",
    "Nh", "Fl", "Mc", "Lv", "Ts",
    "Og"
  };
  string a = "", b= "", c="";
  bool first = isElementSpellable("feline", symbols, a);
  bool second = isElementSpellable("interesting", symbols, b);
  bool third = isElementSpellable("CHeMoWIZrDy", symbols, c);
  EXPECT_EQUAL(isElementSpellable("feline", symbols, a), true);
  EXPECT_EQUAL(isElementSpellable("interesting", symbols, b), false);
  EXPECT_EQUAL(isElementSpellable("CHeMoWIZrDy", symbols, c), true);
}
