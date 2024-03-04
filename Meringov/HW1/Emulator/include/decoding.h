#ifndef ARCH_SPRING_2024_DECODING_H
#define ARCH_SPRING_2024_DECODING_H

#include <cstdint>

namespace decoding {
inline uint32_t opcode(uint32_t inst) {
    return inst >> 26;  // bits 31..26
}

inline uint32_t rs(uint32_t inst) {
    return inst >> 21 & 0x1f;  // bits 25..21
}

inline uint32_t rt(uint32_t inst) {
    return inst >> 16 & 0x1f;  // bits 20..16
}

inline uint32_t rd(uint32_t inst) {
    return inst >> 11 & 0x1f;  // bits 15..11
}

inline uint32_t shamt(uint32_t inst) {
    return inst >> 6 & 0x1f;  // bits 10..6
}

inline uint32_t funct(uint32_t inst) {
    return inst & 0x3f;  // bits 5..0
}

inline uint32_t imm(uint32_t inst) {
    return inst & 0xffff;  // bits 15..0
}

inline uint32_t addr(uint32_t inst) {
    return inst & 0x3ffffff;  // bits 25..0
}
}  // namespace decoding

#endif  // ARCH_SPRING_2024_DECODING_H
