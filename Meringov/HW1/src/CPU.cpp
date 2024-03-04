#include "CPU.h"
#include <iostream>
#include <unordered_map>
#include "decoding.h"
#include "instructions.h"
#include "utils.h"

using instruction = void (CPU::*)(uint32_t);
const std::unordered_map<OPCODES, instruction> opcode_to_instruction = {
    {OPCODES::ADDI, &CPU::ADDI},
    {OPCODES::ANDI, &CPU::ANDI},
    {OPCODES::ORI, &CPU::ORI},
    {OPCODES::XORI, &CPU::XORI},
    {OPCODES::LHI, &CPU::LHI},
    {OPCODES::LLO, &CPU::LLO},
    {OPCODES::SLTI, &CPU::SLTI},
    {OPCODES::BEQ, &CPU::BEQ},
    {OPCODES::BGTZ, &CPU::BGTZ},
    {OPCODES::BLEZ, &CPU::BLEZ},
    {OPCODES::BNE, &CPU::BNE},
    {OPCODES::J, &CPU::J},
    {OPCODES::JR, &CPU::JR},
    {OPCODES::LW, &CPU::LW},
    {OPCODES::SW, &CPU::SW},
};

const std::unordered_map<FUNCT, instruction> funct_to_r_type_instruction = {
    {FUNCT::ADD, &CPU::ADD},
    {FUNCT::AND, &CPU::AND},
    {FUNCT::DIV, &CPU::DIV},
    {FUNCT::MULT, &CPU::MULT},
    {FUNCT::NOR, &CPU::NOR},
    {FUNCT::OR, &CPU::OR},
    {FUNCT::SLT, &CPU::SLT},
    {FUNCT::SLL, &CPU::SLL},
    {FUNCT::SRA, &CPU::SRA},
    {FUNCT::SRL, &CPU::SRL},
    {FUNCT::SUB, &CPU::SUB},
    {FUNCT::XOR, &CPU::XOR},

};

CPU::CPU()
    : bus() {
    registers[29] = bus.get_memory_size();
}
uint32_t CPU::fetch() {
    return bus.load(PC++);
}
void CPU::execute(uint32_t inst) {
    std::cerr << "inst = " << std::bitset<32>(inst).to_string() << ", " << inst << '\n';
    if (inst == 0) {
        return;
    }
    if (inst == ~0) {
        CPU::print(inst);
        return;
    }
    uint32_t opcode = decoding::opcode(inst);

    if (static_cast<OPCODES>(opcode) == OPCODES::R_TYPE) {
        uint32_t funct = decoding::funct(inst);

        if (auto it = funct_to_r_type_instruction.find(static_cast<FUNCT>(funct)); it != funct_to_r_type_instruction.end()) {
            auto function = it->second;
            (this->*function)(inst);
        }

    } else if (auto it = opcode_to_instruction.find(static_cast<OPCODES>(opcode)); it != opcode_to_instruction.end()) {
        auto function = it->second;
        (this->*function)(inst);
    } else {
        exit(1);
    }

    registers[0] = 0;
}

void CPU::print(uint32_t inst) {
    std::cerr << "CPU::print" << '\n';
    std::cout << static_cast<int32_t>(registers[2]);
}

void CPU::R_TYPE(uint32_t inst) {
    std::cerr << "CPU::R_TYPE" << '\n';
    uint32_t funct = decoding::funct(inst);
    using enum FUNCT;
}
void CPU::ADDI(uint32_t inst) {
    std::cerr << "CPU::ADDI" << '\n';
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    uint32_t imm = decoding::imm(inst);

    registers[rt] = registers[rs] + utils::SE(imm);
}

void CPU::ANDI(uint32_t inst) {
    std::cerr << "CPU::ANDI" << '\n';
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    uint32_t imm = decoding::imm(inst);

    registers[rt] = registers[rs] & imm;
}

void CPU::ORI(uint32_t inst) {
    std::cerr << "CPU::ORI" << '\n';
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    uint32_t imm = decoding::imm(inst);

    registers[rt] = registers[rs] | imm;
}

void CPU::XORI(uint32_t inst) {
    std::cerr << "CPU::XORI" << '\n';
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    uint32_t imm = decoding::imm(inst);

    registers[rt] = registers[rs] ^ imm;
}

void CPU::LHI(uint32_t inst) {
    std::cerr << "CPU::LHI" << '\n';
    uint32_t rt = decoding::rt(inst);

    uint32_t imm = decoding::imm(inst);

    registers[rt] = (registers[rt] & 0x0000ffff) | (imm << 16);
}
void CPU::LLO(uint32_t inst) {
    std::cerr << "CPU::LLO" << '\n';
    uint32_t rt = decoding::rt(inst);

    uint32_t imm = decoding::imm(inst);

    registers[rt] = (registers[rt] & 0xffff0000) | imm;
}

void CPU::SLTI(uint32_t inst) {
    std::cerr << "CPU::SLTI" << '\n';
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    uint32_t imm = decoding::imm(inst);

    if (registers[rs] < imm) {
        registers[rt] = 1;
    } else {
        registers[rt] = 0;
    }
}
void CPU::BEQ(uint32_t inst) {
    std::cerr << "CPU::BEQ" << '\n';


    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    std::cerr << "rs = " << registers[rs] << ", rt = " << registers[rt] << '\n';
    uint32_t imm = decoding::imm(inst);

    if (registers[rs] == registers[rt]) {
        uint32_t offset = utils::SE(imm);
        PC += offset /* << 2 */;
    }
}
void CPU::BGTZ(uint32_t inst) {
    std::cerr << "CPU::BGTZ" << '\n';
    uint32_t rs = decoding::rs(inst);

    uint32_t imm = decoding::imm(inst);

    if (registers[rs] >> 31 == 0 && registers[rs] != 0) {  // rs > 0
        uint32_t offset = utils::SE(imm);
        PC += offset /* << 2 */;
    }
}
void CPU::BLEZ(uint32_t inst) {
    std::cerr << "CPU::BLEZ" << '\n';
    uint32_t rs = decoding::rs(inst);

    uint32_t imm = decoding::imm(inst);

    if (registers[rs] == 0 || registers[rs] >> 31 == 1) {  // rs <= 0
        uint32_t offset = utils::SE(imm);
        PC += offset /* << 2 */;
    }
}
void CPU::BNE(uint32_t inst) {
    std::cerr << "CPU::BNE" << '\n';
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    uint32_t imm = decoding::imm(inst);

    if (registers[rs] != registers[rt]) {
        uint32_t offset = utils::SE(imm);
        PC += offset /* << 2 */;
    }
}
void CPU::J(uint32_t inst) {
    std::cerr << "CPU::J" << '\n';
    uint32_t addr = decoding::addr(inst);
    PC += utils::SE(addr, 26);
}
void CPU::JR(uint32_t inst) {
    std::cerr << "CPU::JR" << '\n';
    uint32_t rs = decoding::rs(inst);
    PC = registers[rs];
}

void CPU::LW(uint32_t inst) {
    std::cerr << "CPU::LW" << '\n';
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);
    uint32_t imm = decoding::imm(inst);

    uint32_t offset = utils::SE(imm);
    uint32_t address = registers[rs] + offset;

    registers[rt] = bus.load(address);
}
void CPU::SW(uint32_t inst) {
    std::cerr << "CPU::SW" << '\n';
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);
    uint32_t imm = decoding::imm(inst);

    uint32_t offset = utils::SE(imm);
    uint32_t address = registers[rs] + offset;
    dump_register();
    std::cerr << "$rs -> " << registers[rs] << ", offset = " << offset << '\n';
    std::cerr << "sw -> " << address << '\n';
    bus.save(address, registers[rt]);
}

void CPU::ADD(uint32_t inst) {
    std::cerr << "CPU::ADD" << '\n';
    uint32_t rd = decoding::rd(inst);
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    registers[rd] = registers[rs] + registers[rt];
}

void CPU::AND(uint32_t inst) {
    std::cerr << "CPU::AND" << '\n';
    uint32_t rd = decoding::rd(inst);
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    registers[rd] = registers[rs] & registers[rt];
}

void CPU::DIV(uint32_t inst) {
    std::cerr << "CPU::DIV" << '\n';
    // TODO
}

void CPU::MULT(uint32_t inst) {
    std::cerr << "CPU::MULT" << '\n';
    // TODO
}

void CPU::NOR(uint32_t inst) {
    std::cerr << "CPU::NOR" << '\n';
    uint32_t rd = decoding::rd(inst);
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    registers[rd] = ~(registers[rs] | registers[rt]);
}

void CPU::OR(uint32_t inst) {
    std::cerr << "CPU::OR" << '\n';
    uint32_t rd = decoding::rd(inst);
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    registers[rd] = registers[rs] | registers[rt];
}

void CPU::SLT(uint32_t inst) {
    std::cerr << "CPU::SLT" << '\n';
    uint32_t rd = decoding::rs(inst);
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    if (registers[rs] < registers[rt]) {
        registers[rd] = 1;
    } else {
        registers[rd] = 0;
    }
}

void CPU::SLL(uint32_t inst) {
    std::cerr << "CPU::SLL" << '\n';
    uint32_t rd = decoding::rd(inst);
    uint32_t rt = decoding::rt(inst);
    uint32_t shamt = decoding::shamt(inst);

    registers[rd] = registers[rt] << shamt;
}

void CPU::SRA(uint32_t inst) {
    std::cerr << "CPU::SRA" << '\n';
    uint32_t rd = decoding::rd(inst);
    uint32_t rt = decoding::rt(inst);
    uint32_t shamt = decoding::shamt(inst);

    registers[rd] = registers[rt] >> shamt;
}
void CPU::SRL(uint32_t inst) {
    std::cerr << "CPU::SRL" << '\n';
    // TODO
}
void CPU::SUB(uint32_t inst) {
    std::cerr << "CPU::SUB" << '\n';
    uint32_t rd = decoding::rd(inst);
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    registers[rd] = registers[rs] - registers[rt];
}
void CPU::XOR(uint32_t inst) {
    std::cerr << "CPU::XOR" << '\n';
    uint32_t rd = decoding::rd(inst);
    uint32_t rs = decoding::rs(inst);
    uint32_t rt = decoding::rt(inst);

    registers[rd] = registers[rs] ^ registers[rt];
}
CPU::CPU(const std::vector<uint32_t> &instructions): bus(Bus(instructions)) {
    registers[29] = bus.get_memory_size();
}
