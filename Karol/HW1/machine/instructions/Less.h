#ifndef LESS_H
#define LESS_H

#include "Instruction.h"

#define CMP_REGISTER 2

class Less : Instruction {
public:
    Less() : Instruction(5) {}

    void process(std::bitset<28> command) override {
        uint16_t register1 = bitset_slice<28, 5>(command, 0).to_ullong();
        uint16_t register2 = bitset_slice<28, 5>(command, 5).to_ullong();
        setToRegister(CMP_REGISTER, getRegister(register1) < getRegister(register2));
    }
};

#endif
