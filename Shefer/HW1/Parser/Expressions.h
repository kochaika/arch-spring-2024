#pragma once

#include <map>
#include <string>

enum ExpressionType {
    Variable,
    Const,
    OperandTwo,
    OperandSingle
};

enum OperandTypeSingle {
    NotS,
    MinusS
};

enum OperandTypeTwo {
    Lt,
    Le,
    Gt,
    Ge,
    Eq,
    Plus,
    Minus,
    And,
    Or
};

class Expression {

private:
    ExpressionType expressionType;

public:

    Expression(ExpressionType expressionType);

    ExpressionType getType();

    virtual void print() = 0;
};

class ExpressionVar : public Expression {

private:
    std::string var;

public:
    ExpressionVar(std::string var);

    std::string getVar();

    void print() override;
};


class ExpressionConst : public Expression {

private:
    int x;

public:
    ExpressionConst(int x);

    int getConst();

    void print() override;
};

class ExpressionOperandTwo : public Expression {

private:
    Expression* left;
    Expression* right;

    OperandTypeTwo op;

public:

    ExpressionOperandTwo(Expression* left, Expression* right, OperandTypeTwo op);

    Expression* getLeft();

    Expression* getRight();

    OperandTypeTwo getOpT();

    void print() override;
};


class ExpressionOperandSingle : public Expression {

private:
    Expression* inner;

    OperandTypeSingle op;

public:

    ExpressionOperandSingle(Expression* inner, OperandTypeSingle op);

    Expression* getInner();

    OperandTypeSingle getOpS();

    void print() override;
};




