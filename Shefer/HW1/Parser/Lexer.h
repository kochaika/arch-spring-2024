#pragma once

#include <string>
#include "Token.h"


class Lexer {

private:

    std::string::const_iterator cursor_;
    std::string::const_iterator begin_;
    std::string::const_iterator end_;


    const std::string spaces = " \r\n\t";
    const std::string decimal_digits = "0123456789";
    const std::string delimeters = ",;{}()\\";
    const std::string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    inline bool isLetter(char t) {
        return letters.find(tolower(t)) != std::string::npos;
    }

    inline bool isNumeric(char t) {
        return decimal_digits.find(t) != std::string::npos;
    }

    inline bool isSpace(char t) {
        return spaces.find(t) != std::string::npos;
    }

    inline bool isDelimeter(char t) {
        return delimeters.find(t) != std::string::npos;
    }

    inline bool isSign(char t){
        return std::string("-+").find(t) != std::string::npos;
    }

    inline bool isOperator(char t){
        return (std::string("-+=<>&|").find(t) != std::string::npos);
    }

    inline bool isCompare(char t){
        return std::string("<>=").find(t) != std::string::npos;
    }

    inline bool IsEof() const;
    inline char IncrementCursor();
    inline char CursorValue() const;
    inline char CursorNextValue() const;

    Token* GetNextToken();
    Token* ParseNumber();
    Token* ParseId();
    Token* ParseOperator();
    void SkipSpacesAndComments();

public:

    Lexer();

    bool ParseTokens(std::string text, TokenStream& stream);

};