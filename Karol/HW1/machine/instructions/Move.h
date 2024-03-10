#ifndef MOVE_H
#define MOVE_H

#include "Instruction.h"

class Move : Instruction {
public:
    Move() : Instruction(9) {}

    void process(std::bitset<28> command) override {
        uint8_t register_from = bitset_slice<28, 5>(command, 0).to_ullong();
        uint8_t register_to = bitset_slice<28, 5>(command, 5).to_ullong();
        setToRegister(register_to, getRegister(register_from));
    }
};

#endif