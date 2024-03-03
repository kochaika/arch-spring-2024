#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#include "alu.h"

typedef enum {
  OP_R = 0b00,
  OP_JUMP = 0b01,
  OP_LOAD = 0b10,
  OP_STORE = 0b11,
} OPCODE;

typedef struct {
  OPCODE op_code;
  int r1;
  int r2;
  int address;
  ALU_OP alu_op;
  int intruction_offset;
} Instruction;

Instruction *instruction_fetch(void *instructions, int pc);
void instruction_free(Instruction *instruction);

void instruction_build_r(void *instr, ALU_OP op, int r1, int r2);
void instruction_build_load(void *instr, int r, int address);
void instruction_build_store(void *instr, int r, int address);
void instruction_build_jump(void *instr, int r, int offset);

#endif // !INSTRUCTIONS_H
