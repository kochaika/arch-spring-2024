#include <iostream>
#include "instructions/LoadI.h"
#include <bitset>

#include <sstream>
#include <string>
#include <vector>

class AssemblerSimulator {
public:
    void run(const std::vector<std::string> &code) {
        for (const std::string &line : code) {
            parseLine(line);
        }

        for (const auto &command : asmCode) {
            std::cout << command << std::endl;
        }
    }

private:
    std::vector<std::string> asmCode;
    std::vector<std::string> varStack; // Можно использовать как стек для переменных, если нужно
    int ifLabelCounter = 0;
    int whileLabelCounter = 0;

    void parseLine(const std::string &line) {
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        // Пропускаем пустые строки и комментарии
        if (token.empty() || token[0] == '#') return;

        if (token == "print") {
            iss >> token; // переменная для печати
            asmCode.push_back("PRINT " + token);
        } else if (token == "if") {
            std::string condition;
            getline(iss, condition); // Считываем условие, пропуская "if"
            std::string label = createLabel("IF_END");
            asmCode.push_back("IF_NOT " + condition + " GOTO " + label); // Псевдокоманда, которая предполагает условный переход
            varStack.push_back(label); // Запоминаем метку на стеке
        } else if (token == "while") {
            std::string condition;
            getline(iss, condition); // Считываем условие, пропуская "while"
            std::string startLabel = createLabel("WHILE_START");
            std::string endLabel = createLabel("WHILE_END");
            asmCode.push_back(startLabel + ":");
            asmCode.push_back("IF_NOT " + condition + " GOTO " + endLabel); // Псевдокоманда, которая предполагает условный переход
            varStack.push_back(endLabel); // Запоминаем метку конца цикла
        } else if (token == "{") {
            // Начало блока инструкций, в текущем контексте это не требует действий
        } else if (token == "}") {
            std::string label = varStack.back();
            asmCode.push_back("GOTO " + label); // Генерируем переход на начало цикла для while или на конец для if
            asmCode.push_back(label + ":");
            varStack.pop_back();
        } else {
            // Обработка присваивания и арифметических выражений
            std::string varName = token;
            std::string equalSign;
            iss >> equalSign; // дальше должно быть "="
            std::string expr;
            getline(iss, expr); // выражение для вычисления
            parseExpression(varName, expr);
        }
    }

    void parseExpression(const std::string &varName, const std::string &expr) {
        std::istringstream iss(expr);
        std::string firstOperand, operatorToken, secondOperand;

        iss >> firstOperand; // может быть переменная или число

        if (!(iss >> operatorToken)) { // присвоение без оператора (e.g. "a = b;")
            asmCode.push_back("MOV " + varName + ", " + firstOperand);
            return;
        }

        if (operatorToken == "+" || operatorToken == "-") {
            iss >> secondOperand;

            // Псевдокод для простых операций
            asmCode.push_back("LOAD R0, " + firstOperand);
            std::string operation = operatorToken == "+" ? "ADD" : "SUB";
            asmCode.push_back(operation + " R0, " + secondOperand);
            asmCode.push_back("STORE " + varName + ", R0");
        }
    }

    std::string createLabel(const std::string &baseName) {
        // Создание уникальных меток
        if (baseName == "IF_END") {
            return baseName + "_" + std::to_string(ifLabelCounter++);
        } else if (baseName == "WHILE_START" || baseName == "WHILE_END") {
            return baseName + "_" + std::to_string(whileLabelCounter++);
        }
        return baseName;
    }
};

int main() {
    std::vector<std::string> code = {
            "a = 10",
            "b = a + 15",
            "b = b - 7",
            "while (b > 10) {",
            "while (a > 10) {",
            "    a = a + b",
            "}",
            "}",
            "print a",
            "print b"
    };

    AssemblerSimulator simulator;
    simulator.run(code);
}