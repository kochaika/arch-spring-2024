#include <iostream>
#include "Statements.h"

Statement::Statement(StatementType statementType) : statementType(statementType) {}

StatementType Statement::getStmtType() {
    return statementType;
}


AssignStmt::AssignStmt(std::string left, Expression *right) : Statement(Assign), left(left), right(right) {}

std::string AssignStmt::getLeft() {
    return left;
}

Expression *AssignStmt::getRight() {
    return right;
}

void AssignStmt::print() {
    std::cout << left << " = ";
    right->print();
    std::cout << ":\n";
}


IfStmt::IfStmt(Expression *cond, Statements stmtsThen, Statements stmtsElse) : Statement(If), cond(cond), stmtsThen(stmtsThen), stmtsElse(stmtsElse) {}

Expression *IfStmt::getCond() {
    return cond;
}

Statements IfStmt::getThen() {
    return stmtsThen;
}

Statements IfStmt::getElse() {
    return stmtsElse;
}

void IfStmt::print() {
    std::cout << "if (";
    cond->print();
    std::cout << ") {\n";
    for (auto stmt : stmtsThen) {
        stmt->print();
    }
    if (stmtsElse.size() != 0) {
        std::cout << "} else {\n";
        for (auto stmt : stmtsElse) {
            stmt->print();
        }
    }
    std::cout << "}\n";
}


WhileStmt::WhileStmt(Expression *cond, Statements stmtsBody) : Statement(While), cond(cond), stmtsBody(stmtsBody) {}

Expression *WhileStmt::getCond() {
    return cond;
}

Statements WhileStmt::getBody() {
    return stmtsBody;
}

void WhileStmt::print() {
    std::cout << "while (";
    cond->print();
    std::cout << ") {\n";
    for (auto stmt : stmtsBody) {
        stmt->print();
    }
    std::cout << "}\n";
}


PrintStmt::PrintStmt(Expression *expr) : Statement(Print), expr(expr) {}

Expression *PrintStmt::getExpr() {
    return expr;
}

void PrintStmt::print() {
    std::cout << "print(";
    expr->print();
    std::cout << ");\n";
}


VarStmt::VarStmt(std::string name) : Statement(VarDefinition), name(name) {}

std::string VarStmt::getName() {
    return name;
}

void VarStmt::print() {
    std::cout << "var " << name << ";\n";
}


