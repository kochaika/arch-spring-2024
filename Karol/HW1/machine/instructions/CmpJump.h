#ifndef CMP_JUMP_H
#define CMP_JUMP_H

#include "Instruction.h"

#define CMP_REGISTER 2
#define POINTER_REGISTER 1

class CmpJump : Instruction {
public:
    CmpJump() : Instruction(8) {}

    void process(std::bitset<28> command) override {
        int value = (int) bitset_slice<28, 16>(command, 0).to_ullong();
        if (!getRegister(CMP_REGISTER)) {
            setToRegister(POINTER_REGISTER, value);
        }
    }
};

#endif
