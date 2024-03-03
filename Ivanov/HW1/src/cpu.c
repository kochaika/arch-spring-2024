#include "cpu.h"
#include "alu.h"
#include "instructions.h"
#include "memory.h"
#include "registers.h"
#include <stdio.h>
#include <stdlib.h>

cpu *cpu_init() {
  register_reset();
  memory_reset();
  cpu *cpu_inst = malloc(sizeof(cpu));
  cpu_inst->pc = 0;
  cpu_inst->instructions = NULL;
  return cpu_inst;
}

void cpu_load_instructions(cpu *cpu, void *instr, int intr_num) {
  cpu->instructions = instr;
  cpu->num_of_instructions = intr_num;
}

static void cycle(cpu *cpu) {
  int mem_value;
  /* Stage 1: Fetch */
  Instruction *cur_instruction = instruction_fetch(cpu->instructions, cpu->pc);
  cpu->pc += 2;
  /* Stage 2: Get register values */
  /* fake print */
  if (cur_instruction->r1 == 0) {
    printf("%d\n", register_get(cur_instruction->r2));
    return;
  }
  int data_1 = register_get((size_t)cur_instruction->r1);
  int data_2 = register_get((size_t)cur_instruction->r2);
  // printf("Data: %d, %d, %d %d\n", data_1, data_2, cur_instruction->alu_op,
  //        cur_instruction->op_code);
  if (cur_instruction->op_code == OP_JUMP) {
    /* Actually, a separete add unit can be used, but I'll re-use ALU */
    // data_1 = cpu->pc - 2;
    // data_2 = cur_instruction->intruction_offset << 1;
    data_2 = 0;
    // printf("Offset: %d\n", cur_instruction->intruction_offset);
  }
  /* Stage 3: ALU execution */
  int result = alu_compute(data_1, data_2, cur_instruction->alu_op);
  // printf("Result: %d\n", result);
  /* Satge 4: Memory interaction */
  switch (cur_instruction->op_code) {
  case OP_STORE:
    memory_set((size_t)cur_instruction->address, result);
    break;
  case OP_LOAD:
    result = memory_get((size_t)cur_instruction->address);
    break;
  case OP_JUMP:
    // printf("r1: %d\n", register_get(cur_instruction->r1));
    if (result != 0) {
      cpu->pc += cur_instruction->intruction_offset << 1;
      cpu->pc -= 2;
    }
    break;
  case OP_R:
    break;
  }

  /* Stage 5: write back to registers */
  if (cur_instruction->op_code != OP_JUMP) {
    register_set((size_t)cur_instruction->r1, result);
  }
}

static int clock(cpu *cpu) {
  if (cpu->pc >= (cpu->num_of_instructions << 1))
    return 0;
  return 1;
}

void cpu_execute(cpu *cpu) {
  while (clock(cpu)) {
    // printf("assasdas: %d %d %d %d %d\n", register_get(10), register_get(11),
    //        register_get(2), cpu->pc, cpu->num_of_instructions);
    cycle(cpu);
  }
}

void reset(cpu *cpu_inst) {
  register_reset();
  cpu_inst->pc = 0;
}
