#ifndef Utility_Included
#define Utility_Included

#include "HashFunction.h"
#include <ostream>

/**
 * Macro: DISALLOW_COPYING_OF(Type)
 *
 * Disables copying / assignment of the specified type.
 */
#define DISALLOW_COPYING_OF(Type)                                           \
    Type(const Type &) = delete;                                            \
    Type(Type &&) = delete;                                                 \
    void operator= (Type) = delete

/**
 * Macro: MAKE_PRINTERS_FOR(Type)
 *
 * Exports functions to make the given function printable to cout.
 *
 * If you are defining your own custom type and are getting an error when trying
 * to use MAKE_PRINTERS_FOR, not to worry! Open the file Demos/Printers.cpp. There,
 * you'll find the functions that print out Slot, SlotType, etc. Copy/paste the code
 * there and make appropriate edits to support your own custom types.
 */
#define MAKE_PRINTERS_FOR(Type) \
    friend std::ostream& operator<< (std::ostream&, Type)

/**
 * Macro: MAKE_COMPARATORS_FOR(Type)
 *
 * Exports functions to make the given type comparable with == and !=.
 *
 * If you are defining your own custom type and are getting an error when trying
 * to use MAKE_COMPARATORS_FOR, not to worry! Open the file Demos/Printers.cpp. There,
 * you'll find the functions that compare Slot, etc. Copy/paste the code
 * there and make appropriate edits to support your own custom types.
 */
#define MAKE_COMPARATORS_FOR(Type) \
    friend bool operator== (const Type& lhs, const Type& rhs); \
    friend bool operator!= (const Type& lhs, const Type& rhs) { return !(lhs == rhs); } \
    static_assert(true, "Just so we need a semicolon.")

/* Hash function utilities. */
namespace Hash {
    HashFunction<std::string> random(int numSlots);
    HashFunction<std::string> consistentRandom(int numSlots);    // Randomly chosen, but consistent across runs
    HashFunction<std::string> zero(int numSlots);                // Always zero
    HashFunction<std::string> constant(int numSlots, int value); // Always a constant

    /* Assuming keys are integers. If they aren't, returned hash values will be consistent but arbitrary. */
    HashFunction<std::string> identity(int numSlots);
}

#endif
