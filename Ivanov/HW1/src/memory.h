#ifndef MEMORY_H
#define MEMORY_H
#include <stddef.h>

void memory_set(size_t id, int val);

int memory_get(size_t id);

void memory_reset();

#endif // !MEMORY_H
