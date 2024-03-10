#pragma once

#include "Instructions.h"
#include "../Parser/Statements.h"

class ConverterSM {

private:
    int last_id = 0;

    Label genLabel() {
        return {last_id++};
    }

    Instructions convertExpr(Expression* expr);

    std::pair<bool, Instructions> convertStmt(Label label, Statement* stmt);

    std::pair<bool, Instructions> convertBlock(Label label, Statements stmts);

public:

    Instructions convert(Statements stmts);
};