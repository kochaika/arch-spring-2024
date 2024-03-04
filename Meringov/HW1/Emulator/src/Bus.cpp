#include "Bus.h"
#include <cstdint>
#include <vector>

uint32_t Bus::load(uint32_t addr) {
    return memory[addr];
}

void Bus::save(uint32_t addr, uint32_t value) {
    memory[addr] = value;
}
uint32_t Bus::get_memory_size() {
    return memory.size();
}
Bus::Bus()
    : memory() {
}
Bus::Bus(const std::vector<uint32_t> &instructions)
    : memory() {
    for (size_t i = 0; i < instructions.size(); i++) {
        memory[i] = instructions[i];
    }
}
