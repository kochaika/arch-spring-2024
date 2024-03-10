#pragma once
#include <string>
#include <vector>
#include "Expressions.h"

enum StatementType {
    Assign,
    VarDefinition,
    If,
    While,
    Print
};

class Statement {

private:
    StatementType statementType;
public:

    Statement(StatementType statementType);

    StatementType getStmtType();

    virtual void print() = 0;
};

typedef std::vector<Statement*> Statements;

class AssignStmt : public Statement {

private:
    std::string left;
    Expression* right;

public:
    AssignStmt(std::string left, Expression* right);

    std::string getLeft();

    Expression* getRight();

    void print() override;
};

class IfStmt : public Statement {

private:
    Expression* cond;
    Statements stmtsThen;
    Statements stmtsElse;

public:

    IfStmt(Expression* cond, Statements stmtsThen, Statements stmtsElse);

    Expression* getCond();

    Statements getThen();

    Statements getElse();

    void print() override;
};


class WhileStmt : public Statement {

private:
    Expression* cond;
    Statements stmtsBody;

public:

    WhileStmt(Expression *cond, Statements stmtsBody);

    Expression* getCond();

    Statements getBody();

    void print() override;
};


class PrintStmt : public Statement {

private:
    Expression* expr;

public:

    PrintStmt(Expression *expr);

    Expression* getExpr();

    void print() override;
};

class VarStmt : public Statement {

private:
    std::string name;

public:

    VarStmt(std::string name);

    std::string getName();

    void print() override;
};
