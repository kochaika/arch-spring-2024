#pragma once
#include <string>
#include <vector>

enum SMOps {
    LtO,
    PlusO,
    MinusO,
    AndO,
    OrO,
    XorO,
    NorO
};

enum SmInstType {
    jmpI,
    cjmpI,
    printI,
    ldI,
    stI,
    opI,
    constI,
    labelI
};

class Label {
public:
    int id;

    Label(int id) : id(id) {};

    std::string toString() {
        return "L" + std::to_string(id);
    }
};

enum JmpCond {
    Z,
    NZ
};

class Instruction {

public:
    SmInstType type;

    virtual void print() = 0;

    Instruction(SmInstType type) : type(type) {};
};

class PrintInst : public Instruction {

public:

    PrintInst() : Instruction(printI) {};

    void print() override;
};

class LdInst : public Instruction {
public:

    std::string id;

    LdInst(std::string id) : Instruction(ldI), id(id) {};

    void print() override;
};

class StInst : public Instruction {
public:

    std::string id;

    StInst(std::string id) : Instruction(stI), id(id) {};

    void print() override;
};

class OpInst : public Instruction {
public:

    SMOps op;

    OpInst(SMOps op) : Instruction(opI), op(op) {};

    void print() override;
};

class ConstInst : public Instruction {

public:

    int x;

    ConstInst(int x) : Instruction(constI), x(x) {};

    void print() override;
};


class JmpInst : public Instruction {

public:

    Label label;

    JmpInst(Label label) : Instruction(jmpI), label(label) {};

    void print() override;
};


class CondJmpInst : public Instruction {

public:

    Label label;
    JmpCond jmpCond;

    CondJmpInst(Label label, JmpCond jmpCond) : Instruction(cjmpI), label(label), jmpCond(jmpCond) {};

    void print() override;
};


class LabelInst : public Instruction {

public:

    Label label;

    LabelInst(Label label) : Instruction(labelI), label(label) {};

    void print() override;
};

typedef std::vector<Instruction*> Instructions;