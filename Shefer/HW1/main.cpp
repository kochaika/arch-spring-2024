#include <iostream>
#include <fstream>
#include <sstream>
#include "Parser/Parser.h"
#include "Parser/ParseError.h"
#include "StackMachine/converterSM.h"
#include "Binary/converterBinary.h"

int main(int argc, char* argv[]) {

    std::ifstream in(argv[1], std::ios::in);
    if (in.is_open()) {
        std::stringstream ss;
        ss << in.rdbuf();

        auto* parser = new Parser();
        Statements stmts;
        try {
            stmts = parser->ParseProgram(ss.str());
            for (auto stmt : stmts) {
                stmt->print();
            }
        } catch (ParseError* e) {
            std::cout << e->what();
        }

        std::cout << "\n\n";

        auto converterSM = new ConverterSM();
        auto instructions = converterSM->convert(stmts);

        for (auto inst : instructions) {
            inst->print();
        }

        auto converterBin = new ConverterBinary();
        auto [mem, prog] = converterBin->convert_program(instructions);

        std::ofstream outMem(argv[2]), outProg(argv[3]);

        for (auto byte : mem) {
            outMem << byte;
        }

        for (auto byte : prog) {
            outProg << byte;
        }

        in.close();
        outMem.close();
        outProg.close();
    } else {
        throw std::runtime_error(std::string("Can't open file with program: ") + argv[1]);
    }

    return 0;
}
