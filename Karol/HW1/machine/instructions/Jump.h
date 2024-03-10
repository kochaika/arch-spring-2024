#ifndef JUMP_H
#define JUMP_H

#include "Instruction.h"

#define POINTER_REGISTER 1

class Jump : Instruction {
public:
    Jump() : Instruction(6) {}

    void process(std::bitset<28> command) override {
        uint16_t value = bitset_slice<28, 16>(command, 0).to_ullong();
        setToRegister(POINTER_REGISTER, value);
    }
};

#endif
