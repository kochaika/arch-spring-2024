#include "instructions.h"
#include "alu.h"
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uchr;

static int get_op_code(int raw) { return raw & 0b11; }

static int get_r1(int raw) { return raw >> 2; }

static int get_r2(int raw) { return raw & 0b111111; }

static int get_address(int raw) { return raw; }

static int get_alu_op(int raw) { return raw >> 6; }

static int get_offset(int raw) { return (char)raw; }

Instruction *instruction_fetch(void *instructions, int pc) {
  uchr second_part = ((uchr *)instructions)[pc];
  uchr first_part = ((uchr *)instructions)[pc + 1];

  Instruction *instruction = malloc(sizeof(Instruction));
  instruction->op_code = get_op_code(first_part);
  instruction->r1 = get_r1(first_part);

  instruction->intruction_offset = 0;
  instruction->r2 = 0;
  instruction->alu_op = 0;
  instruction->address = 0;

  switch (instruction->op_code) {
  case OP_R:
    instruction->r2 = get_r2(second_part);
    instruction->alu_op = get_alu_op(second_part);
    break;
  case OP_JUMP:
    instruction->intruction_offset = get_offset(second_part);
    // printf("%d\n", instruction->intruction_offset);
    break;
  case OP_STORE:
  case OP_LOAD:
    instruction->address = get_address(second_part);
    break;
  }

  // printf("%d %d %d\n", first_part, second_part, instruction->address);

  return instruction;
}

void instruction_free(Instruction *instruction) { free(instruction); }

void instruction_build_r(void *instr, ALU_OP op, int r1, int r2) {
  uchr *first_part = (uchr *)instr;
  uchr *second_part = first_part + 1;
  *first_part = (uchr)((op << 6) | r2);
  *second_part = (uchr)((r1 << 2) | OP_R);
}

void instruction_build_load(void *instr, int r, int address) {
  uchr *first_part = (uchr *)instr;
  uchr *second_part = first_part + 1;
  *first_part = (uchr)(address);
  *second_part = (uchr)((r << 2) | OP_LOAD);
  // printf("%d, %d\n", *first_part, *second_part);
}

void instruction_build_store(void *instr, int r, int address) {
  uchr *first_part = (uchr *)instr;
  uchr *second_part = first_part + 1;
  *first_part = (uchr)(address);
  // printf("Store: %d %d\n", *first_part, address);
  *second_part = (uchr)((r << 2) | OP_STORE);
}

void instruction_build_jump(void *instr, int r, int offset) {
  uchr *first_part = (uchr *)instr;
  uchr *second_part = first_part + 1;
  *first_part = (uchr)(offset);
  *second_part = (uchr)((r << 2) | OP_JUMP);
}
