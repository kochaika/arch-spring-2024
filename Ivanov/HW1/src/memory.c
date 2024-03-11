#include "memory.h"
#include <assert.h>

#define NUM_OF_MEMORY_CELLS 512
static int mem_data[NUM_OF_MEMORY_CELLS];

void memory_set(size_t id, int val) {
  assert(id < NUM_OF_MEMORY_CELLS);
  mem_data[id] = val;
}

int memory_get(size_t id) {
  assert(id < NUM_OF_MEMORY_CELLS);
  return mem_data[id];
}

void memory_reset() {
  for (int i = 0; i < NUM_OF_MEMORY_CELLS; i++)
    mem_data[i] = 0;
}
