set( CMAKE_CXX_FLAGS "-O3" )

project(Binary)
cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)

add_library(Binary STATIC
        converterBinary.cpp
        converterBinary.h)

target_link_libraries(Binary ${LIBS})