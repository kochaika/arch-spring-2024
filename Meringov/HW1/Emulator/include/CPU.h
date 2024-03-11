#ifndef CPU_H
#define CPU_H
#include <array>
#include <cstdint>
#include <iostream>
#include "Bus.h"

class CPU {
    std::array<uint32_t, 32> registers;
    uint32_t PC = 0;  // TODO not 0

    Bus bus;

public:
    uint32_t fetch();
    CPU();
    CPU(const std::vector<uint32_t> &instructions);
    inline void dump_register() {
        for (int i = 0; i < registers.size(); i++) {
            std::cerr << "r" << i << " = " << registers[i] << '\n';
        }
    }
    void execute(uint32_t inst);
    void print(uint32_t inst);
    void ADDI(uint32_t inst);
    void ANDI(uint32_t inst);
    void ORI(uint32_t inst);
    void XORI(uint32_t inst);
    void LHI(uint32_t inst);
    void LLO(uint32_t inst);
    void SLTI(uint32_t inst);
    void BEQ(uint32_t inst);
    void BGTZ(uint32_t inst);
    void BLEZ(uint32_t inst);
    void BNE(uint32_t inst);
    void LW(uint32_t inst);
    void SW(uint32_t inst);

    // J-TYPE
    void J(uint32_t inst);
    void JR(uint32_t inst);

    // R-TYPE
    void ADD(uint32_t inst);
    void AND(uint32_t inst);
    void DIV(uint32_t inst);
    void MULT(uint32_t inst);
    void NOR(uint32_t inst);
    void OR(uint32_t inst);
    void SLT(uint32_t inst);
    void SLL(uint32_t inst);
    void SRA(uint32_t inst);
    void SRL(uint32_t inst);
    void SUB(uint32_t inst);
    void XOR(uint32_t inst);
};

#endif  // CPU_H
