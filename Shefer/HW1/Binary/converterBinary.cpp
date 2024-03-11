#include <iostream>
#include "converterBinary.h"

std::vector<std::uint8_t> ConverterBinary::to_bytes(std::uint32_t i) {
//    std::cout << i << "\n";
//    std::cout << (i >> 24) << " " << ((i >> 16) & ((1 << 8) - 1)) << " " << ((i >> 8) & ((1 << 8) - 1)) << " " << (i & ((1 << 8) - 1)) << "\n";
//    for (auto e : {(std::int8_t)(i >> 24), (std::int8_t)((i >> 16) & ((1 << 8) - 1)), (std::int8_t)((i >> 8) & ((1 << 8) - 1)), (std::int8_t)(i & ((1 << 8) - 1))}) {
//        std::cout << e << " ";
//    }
//    std::cout << "\n";
    return {(std::uint8_t)(i >> 24), (std::uint8_t)((i >> 16) & ((1 << 8) - 1)), (std::uint8_t)((i >> 8) & ((1 << 8) - 1)), (std::uint8_t)(i & ((1 << 8) - 1))};
}

std::vector<std::uint8_t> ConverterBinary::pop_from_stack(std::uint8_t r) {
    auto load = to_bytes((new IInst(35, SP, r, 0))->transform()); // lw
    std::uint16_t pos1 = (std::uint16_t) consts_find[1];
    auto load1 = to_bytes((new IInst(35, rZero, rInc, (pos1 << 2)))->transform()); // lw
    auto sub = to_bytes((new RInst(34, SP, rInc, SP))->transform());
    auto res = load;
    res.insert(res.end(), load1.begin(), load1.end());
    res.insert(res.end(), sub.begin(), sub.end());
    return res;
}

std::vector<std::uint8_t> ConverterBinary::push_on_stack(std::uint8_t r) {
    std::uint16_t pos1 = (std::uint16_t) consts_find[1];
    auto load1 = to_bytes((new IInst(35, rZero, rInc, (pos1 << 2)))->transform()); // lw
    auto add = to_bytes((new RInst(35, SP, rInc, SP))->transform());

    auto store = to_bytes((new IInst(43, SP, r, 0))->transform()); // sw
    auto res = load1;
    res.insert(res.end(), add.begin(), add.end());
    res.insert(res.end(), store.begin(), store.end());
    return res;
}

std::vector <std::uint8_t> ConverterBinary::convert_instruction(const Instruction* inst) {
    switch (inst->type) {
        case labelI:
            return {};
        case jmpI: {
            std::uint16_t lab = labels_find[((JmpInst *) inst)->label.id];
            lab <<= 2;
            auto jInst = new JInst(lab);

            return to_bytes(jInst->transform());
        }
        case cjmpI: {
            std::uint16_t lab = labels_find[((CondJmpInst *) inst)->label.id];
            lab <<= 2;
            auto pop = pop_from_stack(r1Op);
            IInst *condInst;
            switch (((CondJmpInst *) inst)->jmpCond) {
                case Z:
                    condInst = new IInst(4, r1Op, rZero, lab);
                    break;
                case NZ:
                    condInst = new IInst(5, r1Op, rZero, lab);
                    break;
            }
            auto iBytes = to_bytes(condInst->transform());
            auto res = pop;
            res.insert(res.end(), iBytes.begin(), iBytes.end());
            return res;
        }
        case opI: {
            auto p1 = pop_from_stack(r1Op);
            auto p2 = pop_from_stack(r2Op);
            auto opInst = (OpInst*)inst;

            std::uint8_t funct = functMap[opInst->op];
            auto rInst = new RInst(funct, r2Op, r1Op, r1Op);
            auto instBytes = to_bytes(rInst->transform());

            auto push = push_on_stack(r1Op);

            auto res = p1;
            res.insert(res.end(), p2.begin(), p2.end());
            res.insert(res.end(), instBytes.begin(), instBytes.end());
            res.insert(res.end(), push.begin(), push.end());

            return res;
        }
        case printI: {
            auto p = pop_from_stack(r1Op);
            auto printInst = to_bytes((new RInst(0, r1Op, 0, 0))->transform());

            auto res = p;
            res.insert(res.end(), printInst.begin(), printInst.end());

            return res;
        }
        case ldI: {
            auto ldInst = (LdInst*)inst;
            auto load = load_var(ldInst->id, r1Op);
            auto push = push_on_stack(r1Op);

            auto res = load;
            res.insert(res.end(), push.begin(), push.end());
            return res;
        }
        case stI: {
            auto stInst = (StInst *) inst;
            auto pop = pop_from_stack(r1Op);
            auto store = store_var(stInst->id, r1Op);

            auto res = pop;
            res.insert(res.end(), store.begin(), store.end());
            return res;
        }
        case constI: {

            auto ldInst = (ConstInst*)inst;
            auto load = load_const(ldInst->x, r1Op);
            auto push = push_on_stack(r1Op);

            auto res = load;
            res.insert(res.end(), push.begin(), push.end());
            return res;
        }
    }
}

std::pair<std::vector<std::uint8_t>, std::vector<std::uint8_t>> ConverterBinary::convert_program(const Instructions& insts) {
    place_labels(insts);
    place_vars(insts);
    place_consts(insts);

    std::vector<std::uint8_t> program_bytes = {};
    for (const auto inst : insts) {
        auto inst_bytes = convert_instruction(inst);
        program_bytes.insert(program_bytes.end(), inst_bytes.begin(), inst_bytes.end());
    }

    std::vector<std::uint8_t> memory_bytes = {};
    for (const auto cnst : consts_mem) {
        auto cur_bytes = to_bytes(cnst);
        memory_bytes.insert(memory_bytes.end(), cur_bytes.begin(), cur_bytes.end());
    }

    return {memory_bytes, program_bytes};
}

void ConverterBinary::place_consts(const Instructions& insts) {
    for (auto inst : insts) {
        if (inst->type == constI) {
            process_const(((ConstInst*)inst)->x);
        }
    }
}

void ConverterBinary::process_const(int x) {
    if (consts_find.find(x) != consts_find.end()) {
        return;
    }
    consts_find[x] = count_const++;
    consts_mem.push_back(x);
}

void ConverterBinary::place_labels(const Instructions& insts) {
    std::uint32_t count_other = 0;
    for (auto inst : insts) {
        if (inst->type == labelI) {
            labels_find[((LabelInst*)inst)->label.id] = count_other + 1;
        } else {
            count_other += size_inst(inst);
        }
    }
}

void ConverterBinary::place_vars(const Instructions& insts) {
    for (auto inst : insts) {
        if (inst->type == stI) {
            process_var(((StInst*)inst)->id);
        } else if (inst->type == ldI) {
            process_var(((LdInst*)inst)->id);
        }
    }
}

void ConverterBinary::process_var(std::string var) {
    if (vars_find.find(var) == vars_find.end()) {
        return;
    }
    vars_find[var] = count_var++;
}

std::vector<std::uint8_t> ConverterBinary::load_var(std::string id, std::uint8_t reg) {
    auto address = (std::uint16_t) vars_find[id];
    auto lw = new IInst(35, rZero, reg, address << 2);
    return to_bytes(lw->transform());
}

std::vector<std::uint8_t> ConverterBinary::store_var(std::string id, std::uint8_t reg) {
    auto address = (std::uint16_t) vars_find[id];
    auto sw = new IInst(43, rZero, reg, address << 2);
    return to_bytes(sw->transform());
}

std::vector<std::uint8_t> ConverterBinary::load_const(std::int32_t x, std::uint8_t reg) {
    auto address = (std::uint16_t) consts_find[x];
    auto lw = new IInst(35, rZero, reg, address << 2);
    return to_bytes(lw->transform());
}

std::uint8_t ConverterBinary::size_inst(const Instruction* inst) {
    std::uint8_t res = 0;
    std::uint8_t push = 3, pop = 3;
    switch (inst->type) {
        case jmpI:
            res = 1;
            break;
        case cjmpI:
            res = pop + 1;
            break;
        case printI:
            res = pop + 1;
            break;
        case ldI:
            res = push + 1;
            break;
        case stI:
            res = pop + 1;
            break;
        case opI:
            res = 2 * pop + 1 + push;
            break;
        case constI:
            res = push + 1;
            break;
        case labelI:
            res = 0;
            break;
    }
    return res;
}

