#pragma once

#include <string>
#include <vector>

enum TokenType {

    EofT,

    ConstantT,
    IdentT,

    OperandT,

    LeftRoundBraceT,
    RightRoundBraceT,
    LeftCurlyBraceT,
    RightCurlyBraceT,
    SemicolonT,

    VarT,
    AssignT,
    PrintT,
    IfT,
    ElseT,
    WhileT,

    IllegalT
};

enum TokenOp {
    PlusTOp,
    MinusTOp,
    AndTOp,
    OrTOp,
    NotTOp,
    LtTOp,
    LeTOp,
    GtTOp,
    GeTOp,
    EqTOp
};

class Token {

private:
    TokenType tokenType;
    std::string name;

public:

    Token(TokenType tokenType, std::string name);

    TokenType getType();

    std::string getName();
};

class TokenConst : public Token {

private:
    int x;

public:

    TokenConst(int x);

    int getConst();
};

class TokenOperand : public Token {

private:
    TokenOp tokenOp;

public:

    TokenOperand(TokenOp tokenOp, std::string name);

    TokenOp getOp();
};

class TokenIdent : public Token {

private:
    std::string id;

public:

    TokenIdent(std::string id);

    std::string getId();
};

typedef std::vector<Token*> TokenStream;