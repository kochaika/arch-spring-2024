#include <iostream>
#include "Lexer.h"
#include "ParseError.h"

Lexer::Lexer() {}

bool Lexer::ParseTokens(std::string text, TokenStream &tokens) {
    tokens.clear();
    begin_ = text.begin();
    end_ = text.end();
    for (cursor_ = begin_; cursor_ != end_; ) {
        tokens.push_back(GetNextToken());
        if (tokens.back()->getType() == IllegalT) {
            throw new ParseError("Can't parse " + tokens.back()->getName());
        }
    }
    return true;
}

bool Lexer::IsEof() const {
//    std::cerr << "IsEof\n";
    return cursor_ == end_;
}

char Lexer::CursorValue() const {
    return (IsEof() ? 0 : *cursor_);
}

char Lexer::CursorNextValue() const {
    return (!IsEof() && (cursor_ + 1 != end_) ? *(cursor_ + 1) : 0);
}

char Lexer::IncrementCursor() {
    if(IsEof()) {
        return 0;
    }
    ++cursor_;
    return (IsEof() ? 0 : *cursor_);
}

void Lexer::SkipSpacesAndComments()
{
//    std::cerr << "SkipSpacesAndComments\n";
    bool ok = false;
    while (!IsEof() && !ok) {
        while (isSpace(CursorValue()))
            IncrementCursor();

        if (CursorValue() == '/') {
            const char symbol = CursorNextValue();
            switch (symbol) {
                case '*':
                    IncrementCursor();
                    while (IncrementCursor()) {
                        if (CursorValue() == '*' && CursorNextValue() == '/') {
                            IncrementCursor();
                            IncrementCursor();
                            break;
                        }
                    }
                    break;
                case '/':
                    IncrementCursor();
                    while (IncrementCursor()) {
                        if (CursorValue() == '\n')
                            break;
                    }
                    break;
                default:
                    ok = true;
            }
        } else {
            ok = true;
        }
    }
}

Token* Lexer::ParseNumber()
{
    char symbol;
    std::string token_text(1, *cursor_);
    while ((symbol = IncrementCursor()) && isNumeric(symbol)) {
        token_text.push_back(symbol);
    }

    return new TokenConst(std::atoi(token_text.c_str()));
}

Token* Lexer::ParseId()
{
    char symbol;
    std::string token_text(1, *cursor_);
    while ((symbol = IncrementCursor()) && isLetter(symbol)) {
        token_text.push_back(symbol);
    }

    if (token_text == "var") return new Token(VarT, token_text);
    else if (token_text == "print") return new Token(PrintT, token_text);
    else if (token_text == "if") return new Token(IfT, token_text);
    else if (token_text == "else") return new Token(ElseT, token_text);
    else if (token_text == "while") return new Token(WhileT, token_text);
    else return new TokenIdent(token_text);
}

Token* Lexer::ParseOperator()
{
    char symbol(*cursor_);
    Token* token;
    switch (symbol) {
        case '+': token = new TokenOperand(PlusTOp, {symbol}); break;
        case '-': token = new TokenOperand(MinusTOp, {symbol}); break;
        case '&': token = new TokenOperand(AndTOp, {symbol}); break;
        case '|': token = new TokenOperand(OrTOp, {symbol}); break;
        case '~': token = new TokenOperand(NotTOp, {symbol}); break;
        case '=':
            if (CursorNextValue() == '=') {
                IncrementCursor();
                token = new TokenOperand(EqTOp, "==");
            } else {
                token = new Token(AssignT, {symbol});
            }
            break;
        case '<':
            if (CursorNextValue() == '=') {
                IncrementCursor();
                token = new TokenOperand(LeTOp, "<=");
            } else {
                token = new TokenOperand(LtTOp, "<");
            }
            break;
        case '>':
            if (CursorNextValue() == '=') {
                IncrementCursor();
                token = new TokenOperand(GeTOp, ">=");
            } else {
                token = new TokenOperand(GtTOp, ">");
            }
            break;
        default:
            token = new Token(IllegalT, {symbol});
    }
    IncrementCursor();
    return token;
}

Token* Lexer::GetNextToken()
{
    SkipSpacesAndComments();

    if (IsEof()) {
        return new Token(EofT, "eof");
    }

    if (isNumeric(*cursor_)) {
        return ParseNumber();
    }

    if (isLetter(*cursor_)) {
        return ParseId();
    }

    if (isDelimeter(*cursor_)) {
        auto* token = new Token(IllegalT, "starting");
        switch (*cursor_) {
            case ';': token = new Token(SemicolonT, {*cursor_}); break;
            case '(': token = new Token(LeftRoundBraceT, {*cursor_}); break;
            case ')': token = new Token(RightRoundBraceT, {*cursor_}); break;
            case '{': token = new Token(LeftCurlyBraceT, {*cursor_}); break;
            case '}': token = new Token(RightCurlyBraceT, {*cursor_}); break;
            default: token = new Token(IllegalT, {*cursor_});
        }
        IncrementCursor();
        return token;
    }

    if (isOperator(*cursor_)) {
        return ParseOperator();
    }

    IncrementCursor();
    return new Token(IllegalT, {*cursor_});
}
