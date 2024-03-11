#include "parser.h"
#include "../alu.h"
#include "../instructions.h"
#include "../memory.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void *parse(char *file_name, int *counter) {
  FILE *file = fopen(file_name, "r");
  char op[20], op_arg1[20], op_arg2[20], *end = NULL;
  int arg1, arg2;
  /* 100 instructions */
  void *instructions = malloc(200);
  memset(instructions, 0, 200);
  size_t memory_id = 0;
  *counter = 0;
  memset(op, 0, sizeof(op));
  if (file == 0) {
    return NULL;
  }

  while (fscanf(file, "%s %s %s", &op, &op_arg1, &op_arg2) == 3) {
    if (isalpha(op_arg1[0])) {
      arg1 = op_arg1[0] - 'a' + 2;
    } else {
      arg1 = strtol(op_arg1, &end, 10);
    }
    if (isalpha(op_arg2[0])) {
      arg2 = op_arg2[0] - 'a' + 2;
    } else {
      end = NULL;
      arg2 = strtol(op_arg2, &end, 10);
    }
    if (strcasecmp(op, "set") == 0) {
      memory_set(memory_id, arg2);
      instruction_build_load(((char *)instructions + *counter), arg1,
                             memory_id);
      memory_id++;
    }
    if (strcasecmp(op, "add") == 0) {
      instruction_build_r(((char *)instructions + *counter), ALU_OP_PLUS, arg1,
                          arg2);
    }
    if (strcasecmp(op, "sub") == 0) {
      instruction_build_r(((char *)instructions + *counter), ALU_OP_MINUS, arg1,
                          arg2);
    }
    if (strcasecmp(op, "and") == 0) {
      instruction_build_r(((char *)instructions + *counter), ALU_OP_AND, arg1,
                          arg2);
    }
    if (strcasecmp(op, "or") == 0) {
      instruction_build_r(((char *)instructions + *counter), ALU_OP_OR, arg1,
                          arg2);
    }
    if (strcasecmp(op, "jmp") == 0) {
      instruction_build_jump(((char *)instructions + *counter), arg1, arg2);
    }
    if (strcasecmp(op, "str") == 0) {
      instruction_build_store(((char *)instructions + *counter), arg1, arg2);
    }
    if (strcasecmp(op, "ld") == 0) {
      instruction_build_load(((char *)instructions + *counter), arg1, arg2);
    }
    /* Special case */
    if (strcasecmp(op, "print") == 0) {
      instruction_build_r(((char *)instructions + *counter), 0, 0, arg1);
    }
    *counter += 2;
    memset(op, 0, sizeof(op));
    // printf("Instruction read %s %d %d!\n", op, arg1, arg2);
  }

  fclose(file);

  return instructions;
}
