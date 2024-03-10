#include "converterSM.h"

std::map<OperandTypeTwo, SMOps> op_map = {{ Lt, LtO },
                                          { Plus, PlusO },
                                          { Minus, MinusO },
                                          { And, AndO },
                                          { Or, OrO }};

Instructions ConverterSM::convertExpr(Expression *expr) {
    switch (expr->getType()) {
        case Variable: {
            return {new LdInst(((ExpressionVar *) expr)->getVar())};
        }
        case Const: {
            return {new ConstInst(((ExpressionConst *) expr)->getConst())};
        }
        case OperandTwo: {
            auto instsL = convertExpr(((ExpressionOperandTwo *) expr)->getLeft());
            auto instsR = convertExpr(((ExpressionOperandTwo *) expr)->getRight());
            Instructions insts = {};
            switch (((ExpressionOperandTwo *) expr)->getOpT()) {
                case Lt:
                    insts = instsL;
                    insts.insert(insts.end(), instsR.begin(), instsR.end());
                    insts.push_back(new OpInst(LtO));
                    break;
                case Le:
                    insts.insert(insts.begin(), new ConstInst(1));

                    insts.insert(insts.end(), instsR.begin(), instsR.end());
                    insts.insert(insts.end(), instsL.begin(), instsL.end());
                    insts.push_back(new OpInst(LtO));

                    insts.push_back(new OpInst(MinusO));
                    break;
                case Gt:
                    insts = instsR;
                    insts.insert(insts.end(), instsL.begin(), instsL.end());
                    insts.push_back(new OpInst(LtO));
                    break;
                case Ge:
                    insts.insert(insts.begin(), new ConstInst(1));

                    insts.insert(insts.end(), instsL.begin(), instsL.end());
                    insts.insert(insts.end(), instsR.begin(), instsR.end());
                    insts.push_back(new OpInst(LtO));

                    insts.push_back(new OpInst(MinusO));
                    break;
                case Eq: {
                    insts.insert(insts.end(), instsL.begin(), instsL.end());
                    insts.insert(insts.end(), instsR.begin(), instsR.end());
                    insts.push_back(new OpInst(XorO));

                    auto label1 = genLabel();
                    auto label2 = genLabel();
                    insts.push_back(new CondJmpInst(label1, NZ));
                    insts.push_back(new ConstInst(1));
                    insts.push_back(new JmpInst(label2));
                    insts.push_back(new LabelInst(label1));
                    insts.push_back(new ConstInst(0));
                    insts.push_back(new LabelInst(label2));

                    break;
                }
                default:
                    insts = instsL;
                    insts.insert(insts.end(), instsR.begin(), instsR.end());
                    insts.push_back(new OpInst(op_map[((ExpressionOperandTwo *) expr)->getOpT()]));
                    break;
            }
            return insts;
        }
        case OperandSingle: {
            auto insts = convertExpr(((ExpressionOperandSingle *) expr)->getInner());
            switch (((ExpressionOperandSingle *) expr)->getOpS()) {
                case NotS:
                    insts.push_back(new ConstInst(-1));
                    insts.push_back(new OpInst(NorO));
                    break;
                case MinusS:
                    insts.insert(insts.begin(), new ConstInst(0));
                    insts.push_back(new OpInst(MinusO));
                    break;
            }
            return insts;
        }
    }
}

std::pair<bool, Instructions> ConverterSM::convertStmt(Label label, Statement* stmt) {
    switch (stmt->getStmtType()) {
        case Assign: {
            auto insts = convertExpr(((AssignStmt *) stmt)->getRight());
            insts.push_back(new StInst(((AssignStmt *) stmt)->getLeft()));
            return {false, insts};
        }
        case VarDefinition: {
            return {false, {}};
        }
        case Print: {
            auto insts = convertExpr(((PrintStmt *) stmt)->getExpr());
            insts.push_back(new PrintInst());
            return {false, insts};
        }
        case If: {
            auto instsC = convertExpr(((IfStmt *) stmt)->getCond());
            auto falseLabel = genLabel();
            instsC.push_back(new CondJmpInst(falseLabel, Z));
            auto instsTrue = convertBlock(label, ((IfStmt *) stmt)->getThen()).second;
            instsC.insert(instsC.end(), instsTrue.begin(), instsTrue.end());
            instsC.push_back(new JmpInst(label));
            instsC.push_back(new LabelInst(falseLabel));
            if (((IfStmt *) stmt)->getElse().size() != 0) {
                auto instsFalse = convertBlock(label, ((IfStmt *) stmt)->getElse()).second;
                instsC.insert(instsC.end(), instsFalse.begin(), instsFalse.end());
            }
            return {true, instsC};
        }
        case While: {
            auto labelBefore = genLabel();
            auto labelAfter = genLabel();
            auto insts = std::vector<Instruction*>{new JmpInst(labelAfter), new LabelInst(labelBefore)};
            auto instsBody = convertBlock(labelAfter, ((WhileStmt *) stmt)->getBody()).second;
            insts.insert(insts.end(), instsBody.begin(), instsBody.end());
            insts.push_back(new LabelInst(labelAfter));
            auto cond = convertExpr(((WhileStmt *) stmt)->getCond());
            insts.insert(insts.end(), cond.begin(), cond.end());
            insts.push_back(new CondJmpInst(labelBefore, NZ));
            return {false, insts};
        }
    }
}

std::pair<bool, Instructions> ConverterSM::convertBlock(Label label, Statements stmts) {
    if (stmts.empty()) {
        return {false, {}};
    }
    Instructions insts = {};
    for (int i = 0; i < stmts.size() - 1; i++) {
        auto label = genLabel();
        auto res = convertStmt(label, stmts[i]);
        insts.insert(insts.end(), res.second.begin(), res.second.end());
        if (res.first) {
            insts.push_back(new LabelInst(label));
        }
    }
    auto res = convertStmt(label, stmts.back());
    insts.insert(insts.end(), res.second.begin(), res.second.end());
    return {res.first, insts};
}

Instructions ConverterSM::convert(Statements stmts) {
    auto label = genLabel();
    auto res = convertBlock(label, stmts);
    auto insts = res.second;
    if (res.first) {
        insts.push_back(new LabelInst(label));
    }
    return insts;
}
