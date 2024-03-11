#include "alu.h"
#include <assert.h>
#include <stdio.h>

int alu_compute(int a, int b, ALU_OP code) {
  switch (code) {
  case ALU_OP_PLUS:
    return a + b;
  case ALU_OP_MINUS:
    return a - b;
  case ALU_OP_AND:
    return a & b;
  case ALU_OP_OR:
    return a | b;
  }
  printf("Error: Unsupported alu op code: %d.\n", code);
  assert(0);
}
