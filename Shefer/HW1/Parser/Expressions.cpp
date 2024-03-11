#include <iostream>
#include "Expressions.h"

std::map<OperandTypeSingle, std::string> to_print1 = {{NotS, "~"}, {MinusS, "-"}};
std::map<OperandTypeTwo, std::string> to_print2 = {{Lt, "<"}, {Le, "<="}, {Gt, ">"}, {Ge, ">="}, {Eq, "=="}, {Plus, "+"}, {Minus, "-"}, {And, "&"}, {Or, "|"}};


Expression::Expression(ExpressionType expressionType) : expressionType(expressionType) {}

ExpressionType Expression::getType() {
    return expressionType;
}


ExpressionVar::ExpressionVar(std::string var) : Expression(Variable), var(var) {}

std::string ExpressionVar::getVar() {
    return var;
}

void ExpressionVar::print() {
    std::cout << var;
}


ExpressionConst::ExpressionConst(int x) : Expression(Const), x(x) {
}

int ExpressionConst::getConst() {
    return x;
}

void ExpressionConst::print() {
    std::cout << x;
}


ExpressionOperandTwo::ExpressionOperandTwo(Expression* left, Expression* right, OperandTypeTwo op) : Expression(OperandTwo), left(left), right(right), op(op) {}

Expression* ExpressionOperandTwo::getLeft() {
    return left;
}

Expression* ExpressionOperandTwo::getRight() {
    return right;
}

void ExpressionOperandTwo::print() {
    std::cout << "(";
    left->print();
    std::cout << to_print2[op];
    right->print();
    std::cout << ")";
}

OperandTypeTwo ExpressionOperandTwo::getOpT() {
    return op;
}


ExpressionOperandSingle::ExpressionOperandSingle(Expression* inner, OperandTypeSingle op) : Expression(OperandSingle), inner(inner), op(op) {}

Expression* ExpressionOperandSingle::getInner() {
    return inner;
}

void ExpressionOperandSingle::print() {
    std::cout << to_print1[op] << "(";
    inner->print();
    std::cout << ")";
}

OperandTypeSingle ExpressionOperandSingle::getOpS() {
    return op;
}

