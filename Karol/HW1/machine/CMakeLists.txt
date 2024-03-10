cmake_minimum_required(VERSION 3.26)
project(machine)

set(CMAKE_CXX_STANDARD 17)

add_executable(machine main.cpp
        instructions/Instruction.h
        storage/GlobalStorage.h
        instructions/LoadI.h
        instructions/Load.h
        instructions/Store.h
        instructions/Add.h
        instructions/Subtract.h
        instructions/Move.h
        instructions/And.h
        instructions/Or.h
        instructions/Xor.h
        instructions/Neg.h
        instructions/Print.h
        instructions/Jump.h
        instructions/Cmp.h
        instructions/CmpJump.h
        instructions/Grt.h
        instructions/Less.h)
