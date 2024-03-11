#include <iostream>
#include "Instructions.h"

void PrintInst::print() {
    std::cout << "PRINT\n";
}


void LdInst::print() {
    std::cout << "LD " << id << "\n";
}

void StInst::print() {
    std::cout << "ST " << id << "\n";
}

void OpInst::print() {
    switch (op) {
        case LtO :
            std::cout << "lt\n";
            break;
        case PlusO:
            std::cout << "add\n";
            break;
        case MinusO:
            std::cout << "sub\n";
            break;
        case AndO:
            std::cout << "and\n";
            break;
        case OrO:
            std::cout << "or\n";
            break;
        case NorO:
            std::cout << "nor\n";
            break;
        case XorO:
            std::cout << "xor\n";
            break;
    }
}

void ConstInst::print() {
    std::cout << "CONST " << x << "\n";
}

void JmpInst::print() {
    std::cout << "JMP " << label.toString() << "\n";
}


void CondJmpInst::print() {
    std::cout << "CJMP ";
    switch (jmpCond) {
        case Z:
            std::cout << "Z ";
            break;
        case NZ:
            std::cout << "NZ ";
            break;
    }
    std::cout << label.toString() << '\n';
}

void LabelInst::print() {
    std::cout << label.toString() << ":\n";
}
