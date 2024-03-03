#include "registers.h"
#include <assert.h>

#define NUM_OF_REGISTERS 64
static int reg_data[NUM_OF_REGISTERS];

void register_set(size_t id, int val) {
  assert(id < NUM_OF_REGISTERS);
  reg_data[id] = val;
}

int register_get(size_t id) {
  assert(id < NUM_OF_REGISTERS);
  return reg_data[id];
}

void register_reset() {
  for (int i = 0; i < NUM_OF_REGISTERS; i++)
    reg_data[i] = 0;
}
