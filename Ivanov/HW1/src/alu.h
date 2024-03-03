#ifndef ALU_H
#define ALU_H

typedef enum {
  ALU_OP_PLUS = 0b00,
  ALU_OP_MINUS = 0b01,
  ALU_OP_AND = 0b10,
  ALU_OP_OR = 0b11,
} ALU_OP;

int alu_compute(int a, int b, ALU_OP code);

#endif // !ALU_H
