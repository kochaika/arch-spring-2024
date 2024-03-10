#ifndef NEG_H
#define NEG_H

#include "Instruction.h"

class Neg : Instruction {
public:
    Neg() : Instruction(11) {}

    void process(std::bitset<28> command) override {
        uint8_t register_ad = bitset_slice<28, 5>(command, 0).to_ullong();
        uint8_t register_add = bitset_slice<28, 5>(command, 10).to_ullong();
        setToRegister(register_add, ~getRegister(register_ad));
    }
};

#endif
