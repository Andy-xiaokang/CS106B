/**
 * The implementation of tabulation hashing provided here is based on an
 * implementation by the legendary Kevin Gibbons when he was a TA for CS166
 * in Spring 2016. Thanks, Kevin!
 */
#include "HashFunction.h"
#include "random.h"
#include <random>
#include <climits>
#include <array>
#include <cstdint>

namespace hashfunction_detail {
    using TabulationTable = std::array<std::array<std::uint32_t, 256>, 4>;

    std::uint32_t random32Bits() {
        static std::mt19937 engine(randomInteger(0, INT_MAX));
        std::uniform_int_distribution<std::uint32_t> dist;
        return dist(engine);
    }

    TabulationTable tabulationTable(std::uint32_t seed) {
        std::mt19937 engine(seed);
        std::uniform_int_distribution<std::uint32_t> dist;

        TabulationTable table;
        for (std::size_t i = 0; i < 4; i++) {
            for (std::size_t byte = 0; byte < 256; byte++) {
                table[i][byte] = dist(engine);
            }
        }
        return table;
    }

    std::function<int(int)> tabulationHashFunction(int seed) {
        auto table = tabulationTable(std::uint32_t(seed));
        return [table] (std::uint32_t key) {
            std::uint32_t result = 0;
            for (size_t i = 0; i < 4; i++) {
                result ^= table[i][(key >> (i * 8)) & 0xFF];
            }
            return result;
        };
    }
}
