#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#define SIZE 28
#define DEFAULT 0

#include <string>
#include <bitset>
#include <iostream>
#include "../storage/GlobalStorage.h"

class Instruction {
private:
    uint8_t name;
public:
    explicit Instruction(uint8_t name) : name(name) {}

    virtual void process(std::bitset<SIZE> command) = 0;

    static int getRegister(uint8_t address) {
        if (registers.find(address) != registers.end()) {
            return DEFAULT;
        }
        return registers[address];
    }

    static void setToRegister(uint8_t address, int value) {
        registers[address] = value;
    }

    static int getAddress(uint16_t address) {
        if (addresses.find(address) != addresses.end()) {
            return DEFAULT;
        }
        return addresses[address];
    }

    static void setToAddress(uint16_t address, int value) {
        addresses[address] = value;
    }

    template<std::size_t N, std::size_t M>
    std::bitset<M> bitset_slice(const std::bitset<N>& input_bitset, std::size_t pos) {
        std::bitset<M> result;
        static_assert(M <= N, "Slice size must be less than or equal to the size of the input bitset.");
        for (std::size_t i = 0; i < M; ++i) {
            if (pos + i < N) {
                result[i] = input_bitset[pos + i];
            }
        }
        return result;
    }

};

#endif
