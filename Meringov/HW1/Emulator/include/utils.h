#ifndef ARCH_SPRING_2024_UTILS_H
#define ARCH_SPRING_2024_UTILS_H
#include <cstdint>

namespace utils {
uint32_t SE(uint32_t imm, uint32_t bit_length = 16) {
    uint32_t result = imm;
    uint32_t sign = imm & (1 << (bit_length - 1));

    uint32_t mask = ~((1 << bit_length) - 1);

    if (sign) {
        result |= mask;
    }
    return result;
}
}  // namespace utils

#endif  // ARCH_SPRING_2024_UTILS_H
