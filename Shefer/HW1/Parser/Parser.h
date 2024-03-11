#pragma once

#include <string>
#include "Statements.h"
#include "Token.h"

class Parser {

private:

    TokenStream tokens_;
    int token_cursor;

    Statements ParseProgram();

    Statement* ParseStmt();

    Expression* ParseExpr();

public:

    Parser();

    Statements ParseProgram(const std::string& text);

};