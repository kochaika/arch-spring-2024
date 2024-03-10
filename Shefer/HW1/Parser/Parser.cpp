
#include <stack>
#include <map>
#include <iostream>
#include "Parser.h"
#include "Lexer.h"
#include "ParseError.h"

Parser::Parser() : token_cursor(0) {}

Statements Parser::ParseProgram(const std::string &text) {
    Lexer lexer;

    if (!lexer.ParseTokens(text, tokens_)) {
        throw new ParseError("Some error occurred while parse tokens.");
    }
    return ParseProgram();
}

Statements Parser::ParseProgram() {

    Statements statements = std::vector<Statement*>();

    for (;;) {
        if (tokens_[token_cursor]->getType() == EofT) {
            break;
        }
        if (Statement* stmt = ParseStmt()) {
            statements.push_back(stmt);
        }
    }

    return statements;
}

Expression* Parser::ParseExpr() {
    std::stack <Token*> control;
    std::vector <Expression*> exprs;

    std::map<TokenOp, OperandTypeTwo> parseT =
            {{PlusTOp, Plus}, {MinusTOp, Minus}, {AndTOp, And}, {OrTOp, Or}, {LtTOp, Lt}, {LeTOp, Le}, {GtTOp, Gt}, {GeTOp, Ge}, {EqTOp, Eq}};

    std::map<TokenOp, int> priority =
            {{PlusTOp, 1}, {MinusTOp, 1}, {AndTOp, 3}, {OrTOp, 4}, {LtTOp, 2}, {LeTOp, 2}, {GtTOp, 2}, {GeTOp, 2}, {EqTOp, 2}};

    for (;;) {

        if (tokens_[token_cursor]->getType() == SemicolonT) {
            while (!control.empty()) {
                auto* topT = (TokenOperand*)control.top();
                OperandTypeTwo tp = parseT[topT->getOp()];
                auto* exprR = exprs[exprs.size() - 1];
                auto* exprL = exprs[exprs.size() - 2];
                auto* curExpr = new ExpressionOperandTwo(exprL, exprR, tp);
                exprs.pop_back();
                exprs.pop_back();
                exprs.push_back(curExpr);
                control.pop();
            }
//            //std::cerr << "Break here\n" << " " << tokens_[token_cursor]->getName() << "\n";
            break;
        }
        //std::cerr << "Expr " << tokens_[token_cursor]->getName() << "\n";
        //std::cerr << control.size() << " " << exprs.size() << "\n";
        //std::cerr << tokens_[token_cursor]->getType() << "\n";
        bool toBreak = false;
        switch (tokens_[token_cursor++]->getType()) {
            case ConstantT : {
//                //std::cerr << "A\n";
                exprs.push_back(new ExpressionConst(((TokenConst*) tokens_[token_cursor - 1])->getConst()));
                break;
            }
            case IdentT : {
                exprs.push_back(new ExpressionVar(((TokenIdent*) tokens_[token_cursor - 1])->getId()));
                break;
            }
            case OperandT : {
                auto* cur = (TokenOperand*)tokens_[token_cursor - 1];
                while (!control.empty() && control.top()->getType() != LeftRoundBraceT && priority[((TokenOperand*)control.top())->getOp()] <=
                                                                                                  priority[cur->getOp()]) {
                    auto* topT = (TokenOperand*)control.top();
                    OperandTypeTwo tp = parseT[topT->getOp()];
                    auto* exprR = exprs[exprs.size() - 1];
                    auto* exprL = exprs[exprs.size() - 2];
                    auto* curExpr = new ExpressionOperandTwo(exprL, exprR, tp);
                    exprs.pop_back();
                    exprs.pop_back();
                    exprs.push_back(curExpr);
                    control.pop();
                }
                control.push(cur);
                break;
            }
            case LeftRoundBraceT : {
                control.push(tokens_[token_cursor - 1]);
                if (tokens_[token_cursor]->getType() == OperandT) {
                    auto* op = (TokenOperand*) tokens_[token_cursor];
                    token_cursor ++;
                    if (tokens_[token_cursor]->getType() != LeftRoundBraceT) {
                        throw new ParseError("Waiting for left round brace");
                    }
                    if (op->getOp() == MinusTOp || op->getOp() == NotTOp) {
                        token_cursor++;
                        auto* expr = ParseExpr();
                        OperandTypeSingle tp;
                        switch (op->getOp()) {
                            case MinusTOp : {
                                tp = MinusS;
                                break;
                            }
                            case NotTOp : {
                                tp = NotS;
                                break;
                            }
                        }
                        exprs.push_back(new ExpressionOperandSingle(expr, tp));
                        if (tokens_[token_cursor]->getType() != RightRoundBraceT) {
                            throw new ParseError("Waiting for right round brace");
                        }
                        token_cursor++;
                    } else {
                        throw new ParseError("Waiting for minus or not");
                    }
                }
                break;
            }
            case RightRoundBraceT : {
                while (!control.empty() && control.top()->getType() != LeftRoundBraceT) {
                    auto* topT = (TokenOperand*)control.top();
                    OperandTypeTwo tp = parseT[topT->getOp()];
                    auto* exprR = exprs[exprs.size() - 1];
                    auto* exprL = exprs[exprs.size() - 2];
                    auto* curExpr = new ExpressionOperandTwo(exprL, exprR, tp);
                    exprs.pop_back();
                    exprs.pop_back();
                    exprs.push_back(curExpr);
                    control.pop();
                }
                if (control.size() == 0) {
                    toBreak = true;
                    token_cursor--;
                    break;
                }
                control.pop();
                break;
            }
        }
        if (toBreak) {
            break;
        }
    }
    if (exprs.size() != 1) {
        throw new ParseError("Should be only 1 expr");
    }
    return exprs.back();
}

Statement* Parser::ParseStmt() {
    //std::cerr << "ParseStmt\n";
    Statement* stmt;
    Token* token = tokens_[token_cursor++];

//    std::cerr << "A " << token->getName() << "\n";
    switch (token->getType()) {
        case VarT : { // declaration
            //std::cerr << "VarT\n";
            if (token_cursor + 2 > tokens_.size() || tokens_[token_cursor]->getType() != IdentT || tokens_[token_cursor + 1]->getType() != SemicolonT) {
                throw new ParseError("Can't parse var declaration");
            }
            stmt = new VarStmt(((TokenIdent*)tokens_[token_cursor])->getId());
            //std::cerr << "stmt\n";
            token_cursor += 2;
            break;
        }
        case PrintT : { // print stmt
            //std::cerr << "PrintT\n";
            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != LeftRoundBraceT) {
                throw new ParseError("Can't parse print");
            }
            token_cursor++;
            Expression* exp = ParseExpr();
            if (token_cursor + 2 > tokens_.size() || tokens_[token_cursor]->getType() != RightRoundBraceT || tokens_[token_cursor + 1]->getType() != SemicolonT) {
                throw new ParseError("Can't parse print");
            }
            token_cursor += 2;
            stmt = new PrintStmt(exp);
            break;
        }
        case WhileT : { // while stmt
            //std::cerr << "WhileT\n";

            // condition
            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != LeftRoundBraceT) {
                throw new ParseError("Can't parse while");
            }
            token_cursor++;
            Expression* exp = ParseExpr();
            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != RightRoundBraceT) {
                throw new ParseError("Can't parse while");
            }
            token_cursor += 1;

            // body
            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != LeftCurlyBraceT) {
                throw new ParseError("Can't parse while");
            }
            token_cursor++;

            Statements stmtsBody;
            while (token_cursor < tokens_.size() && tokens_[token_cursor]->getType() != RightCurlyBraceT) {
                stmtsBody.push_back(ParseStmt());
            }

            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != RightCurlyBraceT) {
                throw new ParseError("Can't parse while");
            }
            token_cursor++;

            stmt = new WhileStmt(exp, stmtsBody);
            break;
        }
        case IfT : { // if stmt

            //std::cerr << "IfT\n";

            // condition
            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != LeftRoundBraceT) {
                throw new ParseError("Can't parse if");
            }
            token_cursor++;
            Expression* exp = ParseExpr();
            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != RightRoundBraceT) {
                throw new ParseError("Can't parse if");
            }
            token_cursor += 1;

            // body
            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != LeftCurlyBraceT) {
                throw new ParseError("Can't parse if");
            }
            token_cursor++;

            Statements stmtsThen, stmtsElse;
            while (token_cursor < tokens_.size() && tokens_[token_cursor]->getType() != RightCurlyBraceT) {
                stmtsThen.push_back(ParseStmt());
            }

            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != RightCurlyBraceT) {
                throw new ParseError("Can't parse if");
            }
            token_cursor++;

            if (token_cursor + 1 < tokens_.size() && tokens_[token_cursor]->getType() == ElseT) {
                token_cursor++;
                if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != LeftCurlyBraceT) {
                    throw new ParseError("Can't parse if");
                }
                token_cursor++;

                while (token_cursor < tokens_.size() && tokens_[token_cursor]->getType() != RightCurlyBraceT) {
                    stmtsElse.push_back(ParseStmt());
                }

                if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != RightCurlyBraceT) {
                    throw new ParseError("Can't parse if");
                }
                token_cursor++;
            }

            stmt = new IfStmt(exp, stmtsThen, stmtsElse);
            break;
        }
        case IdentT : { // assign

            //std::cerr << "IdentT\n";
            std::string x = ((TokenIdent*) token)->getId();
            //std::cerr << x;

            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != AssignT) {
                //std::cerr <<tokens_[token_cursor]->getName();
                throw new ParseError("Can't parse assign");
            }
            token_cursor++;

            //std::cerr << "IdentT1\n";

            Expression* exp = ParseExpr();

            //std::cerr << "parsed expr\n";
            if (token_cursor + 1 > tokens_.size() || tokens_[token_cursor]->getType() != SemicolonT) {
                //std::cerr << "Err " << tokens_[token_cursor]->getName() << "\n";
                throw new ParseError("Can't parse assign");
            }
            token_cursor++;

            stmt = new AssignStmt(x, exp);
            break;
        }
        default: {
            throw new ParseError("unexpected token");
        }
    }
    return stmt;
}

