set( CMAKE_CXX_FLAGS "-O3" )

project(Parser)
cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)


add_library(Parser STATIC
        Parser.cpp
        Lexer.cpp
        Parser.h
        Lexer.h
        Expressions.cpp
        Expressions.h
        Statements.cpp
        Statements.h
        Token.cpp
        Token.h
        ParseError.h)


target_link_libraries(Parser ${LIBS})


#project(TransalgParser)
#cmake_minimum_required(VERSION 3.16)
#
#include(src/stdafx.h)
#
#add_library(transalgParser STATIC
#        Expression.h
#        ParseError.h
#        Parser.h
#        Serializer.h
#        Statement.h
#        StatementArrayDefinition.h
#        StatementBlock.h
#        StatementFor.h
#        StatementFunctionDefinition.h
#        StatementIf.h
#        StatementReturn.h
#        StatementTable.h
#        StatementVarDefinition.h
#        Types.h
#        src/Lexer.h
#        src/Expression.cpp
#        src/Lexer.cpp
#        src/ParseError.cpp
#        src/Parser.cpp
#        src/Serializer.cpp
#        src/StatementArrayDefinition.cpp
#        src/StatementFor.cpp
#        src/StatementFunctionDefinition.cpp
#        src/StatementIf.cpp
#        src/StatementReturn.cpp
#        src/StatementTable.cpp
#        src/StatementVarDefinition.cpp
#        StatementEncodingDefinition.h src/StatementEncodingDefinition.cpp)
#
#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    target_compile_definitions(transalgParser PUBLIC DEBUG_MODE)
#elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#    target_compile_definitions(transalgParser PUBLIC QT_NO_DEBUG_OUTPUT)
#endif()
