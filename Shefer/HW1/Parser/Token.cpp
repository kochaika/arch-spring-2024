#include "Token.h"

Token::Token(TokenType tokenType, std::string name) : tokenType(tokenType), name(name) {}

TokenType Token::getType() {
    return tokenType;
}

std::string Token::getName() {
    return name;
}

TokenConst::TokenConst(int x) : Token(ConstantT, std::to_string(x)), x(x) {}

int TokenConst::getConst() {
    return x;
}


TokenIdent::TokenIdent(std::string id) : Token(IdentT, id), id(id) {}

std::string TokenIdent::getId() {
    return id;
}

TokenOperand::TokenOperand(TokenOp tokenOp, std::string name) : Token(OperandT, name), tokenOp(tokenOp) {}

TokenOp TokenOperand::getOp() {
    return tokenOp;
}


