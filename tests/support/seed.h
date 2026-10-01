#ifndef GRAPHLIB_TEST_SEED_H
#define GRAPHLIB_TEST_SEED_H

#include <cstdint>
#include <cstdlib>
#include <limits>
#include <string>

namespace graphlib::test {

inline std::uint64_t seed() {
    const char* value = std::getenv("GRAPHLIB_TEST_SEED");
    if (value == nullptr || *value == '\0') return 0xC0FFEEULL;
    try {
        return std::stoull(value);
    } catch (...) {
        return 0xC0FFEEULL;
    }
}

inline std::uint64_t seed_for_case(std::uint64_t offset) {
    constexpr std::uint64_t prime = 0x9E3779B97F4A7C15ULL;
    return seed() + offset * prime;
}

} // namespace graphlib::test

#endif
