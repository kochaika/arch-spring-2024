set( CMAKE_CXX_FLAGS "-O3" )

project(StackMachine)
cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)

add_library(StackMachine STATIC
        Instructions.cpp
        Instructions.h
        converterSM.cpp
        converterSM.h)

target_link_libraries(StackMachine ${LIBS})