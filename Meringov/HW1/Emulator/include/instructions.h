#ifndef ARCH_SPRING_2024_INSTRUCTIONS_H
#define ARCH_SPRING_2024_INSTRUCTIONS_H

// NOLINTNEXTLINE
enum class OPCODES {
    R_TYPE = 0b000000,  // ?

    // ARITHMETIC AND LOGIC
    ADDI = 0b001000,
    ANDI = 0b001100,
    ORI = 0b001101,
    XORI = 0b001110,

    // CONSTANTS
    LHI = 0b011001,  // HH ($t) = i
    LLO = 0b011000,  // LH ($t) = i

    // COMPARISON
    SLTI = 0b001010,

    // BRANCH
    BEQ = 0b000100,
    BGTZ = 0b000111,
    BLEZ = 0b000110,
    BNE = 0b000101,

    // JUMP
    J = 0b000010,
    JR = 0b001000,

    // LOAD/STORE
    LW = 0b100011,
    SW = 0b101011,
};

// NOLINTNEXTLINE
enum class FUNCT : uint32_t {
    ADD = 0b100000,
    AND = 0b100100,
    DIV = 0b011010,
    MULT = 0b011000,
    NOR = 0b100111,
    OR = 0b100101,
    SLT = 0b101010,
    SLL = 0b000000,  // $rd = $rt << SHAMT
    SRA = 0b000011,  // $rd = $rt >> SHAMT
    SRL = 0b000010,  // $rd = $rt >>> SHAMT
    SUB = 0b100010,
    XOR = 0b100110,
};

#endif