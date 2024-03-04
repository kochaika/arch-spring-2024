#ifndef ARCH_SPRING_2024_BUS_H
#define ARCH_SPRING_2024_BUS_H
#include <array>
#include <cstdint>

class Bus {
    std::array<uint32_t, 1024 * 1024> memory;

public:
    Bus();
    Bus(const std::vector<uint32_t> &instructions);
    uint32_t load(uint32_t addr);
    void save(uint32_t addr, uint32_t value);
    uint32_t get_memory_size();
};

#endif  // ARCH_SPRING_2024_BUS_H
