#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include "../StackMachine/Instructions.h"

enum InstType {
    JType,
    IType,
    RType
};

class Inst {

public:
    InstType instType;
    std::uint8_t opcode; // 6

    Inst(InstType instType, std::uint8_t opcode) : instType(instType), opcode(opcode) {}

    virtual std::uint32_t transform() = 0;

    std::uint32_t trunc_reg(std::uint8_t i) {
        return (std::uint32_t) (i & 0x1f);
    }

};

class RInst : public Inst {

public:
    std::uint8_t rs; // 5
    std::uint8_t rt; // 5
    std::uint8_t rd; // 5
    std::uint8_t funct; // 6

    RInst(std::uint8_t funct, std::uint8_t rs, std::uint8_t rt, std::uint8_t rd) : Inst(RType, 0), rs(rs), rt(rt), rd(rd), funct(funct) {}

    std::uint32_t transform() {
        return (0 << 26) | (trunc_reg(rs) << 21) | (trunc_reg(rt) << 16) | (trunc_reg(rd) << 11) | (0 << 5) | ((std::uint32_t) funct);
    }
};

class JInst : public Inst {

public:
    std::uint32_t address; // 26

    JInst(std::uint32_t address) : Inst(JType, 2), address(address) {}

    std::uint32_t transform() {
        return (2 << 26) | address;
    }
};

class IInst : public Inst {

public:
    std::uint8_t rs; // 5
    std::uint8_t rt; // 5
    std::uint16_t imm; // 16

    IInst(std::uint8_t opcode, std::uint8_t rs, std::uint8_t rt, std::uint16_t imm) : Inst(IType, opcode), rs(rs), rt(rt), imm(imm) {}

    std::uint32_t transform() {
        return (((std::uint32_t) opcode) << 26) | (trunc_reg(rs) << 21) | (trunc_reg(rt) << 16) | imm;
    }
};

class ConverterBinary {

private:

// funct map
    std::map <SMOps, std::uint8_t> functMap = {
            {PlusO, 32}, {MinusO, 34}, {AndO, 36}, {OrO, 37}, {XorO, 38}, {NorO, 39}, {LtO, 42}
    };

// registers
    const std::uint8_t rZero = 0;
    const std::uint8_t r1Op = 8;
    const std::uint8_t r2Op = 9;
    const std::uint8_t rLoad = 10;

    const std::uint8_t rInc = 26;
    const std::uint8_t SP = 29;

// constants placement

    std::vector <std::uint32_t> consts_mem;
    std::map <std::uint32_t, std::uint32_t> consts_find;

// variables placement

    std::map <std::string, std::uint32_t> vars_find;

// label pointers

    std::map <std::uint32_t, std::uint32_t> labels_find;

// collect consts
    std::uint32_t count_const = 0;
    void place_consts(const Instructions& insts);
    void process_const(int x);

// collect labels
    std::uint8_t size_inst(const Instruction* inst);
    void place_labels(const Instructions& insts);

// collect vars
    std::uint32_t count_var = 0;
    void place_vars(const Instructions& insts);
    void process_var(std::string var);

    std::vector <std::uint8_t> to_bytes(std::uint32_t x);

    std::vector <std::uint8_t> convert_instruction(const Instruction* inst);

    std::vector <std::uint8_t> pop_from_stack(std::uint8_t r);

    std::vector <std::uint8_t> push_on_stack(std::uint8_t r);

    std::vector <std::uint8_t> load_var(std::string id, std::uint8_t reg);

    std::vector <std::uint8_t> store_var(std::string id, std::uint8_t reg);

    std::vector <std::uint8_t> load_const(std::int32_t x, std::uint8_t reg);
public:

    ConverterBinary() {
        consts_mem = {};
        consts_find = {};
        vars_find = {};
        labels_find = {};
    }

    std::pair<std::vector<std::uint8_t>, std::vector<std::uint8_t>> convert_program(const Instructions& insts);

};