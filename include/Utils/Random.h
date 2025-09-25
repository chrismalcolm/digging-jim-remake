#pragma once
#include <random>

namespace Utils {

    /**
     * @brief Generate a random integer within a given range.
     *
     * Uses a Mersenne Twister engine seeded with a random device to
     * provide pseudo-random numbers uniformly distributed in [min, max].
     *
     * @param min The inclusive lower bound of the range.
     * @param max The inclusive upper bound of the range.
     * @return int A random integer between @p min and @p max (inclusive).
     *
     * @note The generator is static and persists across calls for efficiency.
     */
    inline int randomInteger(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(min, max);
        return dist(gen);
    }
}
