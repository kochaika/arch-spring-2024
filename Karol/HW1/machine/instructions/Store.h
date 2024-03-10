#ifndef STORE_H
#define STORE_H

#include "Instruction.h"

class Store : Instruction {
public:
    Store() : Instruction(15) {}

    void process(std::bitset<28> command) override {
        uint16_t address = bitset_slice<28, 5>(command, 0).to_ullong();
        uint8_t register_ad = bitset_slice<28, 12>(command, 5).to_ullong();
        setToAddress(address, getRegister(register_ad));
    }
};

#endif
