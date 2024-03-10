#ifndef PRINT_H
#define PRINT_H

#include "Instruction.h"

class Print : Instruction {
public:
    Print() : Instruction(14) {}

    void process(std::bitset<28> command) override {
        uint8_t register_ad = bitset_slice<28, 5>(command, 0).to_ullong();
        std::cout << getRegister(register_ad);
    }
};

#endif
