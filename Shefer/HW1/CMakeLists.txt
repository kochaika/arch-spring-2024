cmake_minimum_required(VERSION 3.21)
project(HW1)

add_subdirectory(Parser)
add_subdirectory(StackMachine)
add_subdirectory(Binary)
set(CMAKE_CXX_STANDARD 23)

add_executable(HW1 main.cpp)

target_link_libraries(HW1
        PUBLIC Parser
        PUBLIC StackMachine
        PUBLIC Binary)
