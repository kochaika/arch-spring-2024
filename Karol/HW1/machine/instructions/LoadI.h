#ifndef LOADI_H
#define LOADI_H

#include "Instruction.h"

class LoadI : Instruction {
public:
    LoadI() : Instruction(1) {}

    void process(std::bitset<28> command) override {
        uint8_t register_ad = bitset_slice<28, 5>(command, 0).to_ullong();
        int value = (int) bitset_slice<28, 16>(command, 5).to_ullong();
        setToRegister(register_ad, value);
    }
};

#endif
