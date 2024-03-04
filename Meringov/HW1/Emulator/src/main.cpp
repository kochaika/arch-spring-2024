#include <fstream>
#include <iostream>
#include "CPU.h"

void text(char *filename);
void binary(char *filename);

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        std::cerr << "No input file\n";
        return 1;
    }
    text(argv[1]);
}

void text(char *filename) {
    std::ifstream file = std::ifstream(filename);


    std::vector<uint32_t> v;
    while (!file.eof()) {
        std::string line;
        file >> line;

        std::bitset<32> inst(line);

        v.push_back(inst.to_ulong());
    }
    CPU cpu(v);
    while (true) {
        uint32_t inst = cpu.fetch();

        if(inst == 0){
            break;
        }

        cpu.execute(inst);

    }

    //    cpu.dump_register();
}
void binary(char *filename) {
    std::ifstream file = std::ifstream(filename, std::ios_base::binary);

    CPU cpu;
    //    file >> skip;
    while (!file.eof()) {
        uint32_t inst;
        file.read(reinterpret_cast<char *>(&inst), sizeof(inst));
        if (file.eof()) {
            break;
        }
        if (file.fail()) {
            std::cout << "File read error!";
            exit(1);
        }

        cpu.execute(inst);
    }
    cpu.dump_register();
}
