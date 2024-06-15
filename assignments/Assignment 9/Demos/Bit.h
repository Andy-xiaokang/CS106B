#pragma once

#include <ostream>

/**
 * Type representing a single bit. You can create a bit in one of many ways:
 *
 *     Bit zero = 0;
 *     Bit zero(0);
 *
 *     queue.enqueue(0);
 *     queue.enqueue(Bit(1));
 *
 *     if (bit == 0) { ... }
 */
class Bit {
public:
    Bit() = default;
    Bit(int value);

    friend bool operator== (Bit lhs, Bit rhs);
    friend bool operator!= (Bit lhs, Bit rhs);
    friend std::ostream& operator<< (std::ostream& out, Bit bit);

private:
    bool mValue;
};
