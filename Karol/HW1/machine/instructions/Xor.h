#ifndef XOR_H
#define XOR_H

#include "Instruction.h"

class Xor : Instruction {
public:
    Xor() : Instruction(13) {}

    void process(std::bitset<28> command) override {
        uint8_t register_ad1 = bitset_slice<28, 5>(command, 0).to_ullong());
        uint8_t register_ad2 = (bitset_slice<28, 5>(command, 5).to_ullong());
        uint8_t register_add = (bitset_slice<28, 5>(command, 10).to_ullong());
        setToRegister(register_add, getRegister(register_ad1) ^ getRegister(register_ad2));
    }
};

#endif
