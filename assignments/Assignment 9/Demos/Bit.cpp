#include "Bit.h"
#include "error.h"
#include <string>
using namespace std;

Bit::Bit(int value) {
    /* Check for use of chararacter values. */
    if (value == '0' || value == '1') {
        error("You have attempted to create a bit equal to the character '0' or '1'. "
              "The characters '0' and '1' are not the same as the numbers 0 and 1. "
              "Edit your code to instead use the numeric values 0 and 1 instead.");
    }
    if (value != 0 && value != 1) {
        error("Illegal value for a bit: " + to_string(value));
    }

    mValue = (value == 1);
}

bool operator== (Bit lhs, Bit rhs) {
    return lhs.mValue == rhs.mValue;
}
bool operator!= (Bit lhs, Bit rhs) {
    return !(lhs == rhs);
}
ostream& operator<< (ostream& out, Bit bit) {
    return out << (bit.mValue? '1' : '0');
}
