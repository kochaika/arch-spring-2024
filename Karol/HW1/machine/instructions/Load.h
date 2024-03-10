#ifndef LOAD_H
#define LOAD_H

#include "Instruction.h"

class Load : Instruction {
public:
    Load() : Instruction(3) {}

    void process(std::bitset<28> command) override {
        uint16_t address = bitset_slice<28, 12>(command, 0).to_ullong();
        uint8_t register_ad = bitset_slice<28, 5>(command, 12).to_ullong();
        setToRegister(register_ad, getAddress(address));
    }
};

#endif
