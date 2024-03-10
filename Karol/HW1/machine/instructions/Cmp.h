#ifndef CMP_H
#define CMP_H

#include "Instruction.h"

#define CMP_REGISTER 2

class Cmp : Instruction {
public:
    Cmp() : Instruction(2) {}

    void process(std::bitset<28> command) override {
        uint16_t register1 = bitset_slice<28, 5>(command, 0).to_ullong();
        uint16_t register2 = bitset_slice<28, 5>(command, 5).to_ullong();
        setToRegister(CMP_REGISTER, getRegister(register1) == getRegister(register2));
    }
};

#endif
