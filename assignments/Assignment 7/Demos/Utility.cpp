#include "Utility.h"
#include <cstdlib>
#include <cstdint>
#include <array>
#include <random>
#include <climits>

/* Random number seed to use when asking for a consistent hash function. */
const int kStableSeed = 137;

HashFunction<std::string> Hash::random(int numSlots) {
    return HashFunction<std::string>(numSlots);
}

HashFunction<std::string> Hash::consistentRandom(int numSlots) {
    return HashFunction<std::string>(numSlots, kStableSeed);
}

HashFunction<std::string> Hash::zero(int numSlots) {
    return constant(numSlots, 0);
}

HashFunction<std::string> Hash::constant(int numSlots, int value) {
    return HashFunction<std::string>::wrap(numSlots, [value](const std::string&) { return value; });
}

HashFunction<std::string> Hash::identity(int numSlots) {
    return HashFunction<std::string>::wrap(numSlots, [](const std::string& str) { return strtol(str.c_str(), nullptr, 10); });
}
