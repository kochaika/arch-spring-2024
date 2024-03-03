#ifndef REGISTERS_H
#define REGISTERS_H
#include <stddef.h>

void register_set(size_t id, int val);
int register_get(size_t id);
void register_reset();

#endif // !REGISTERS_H
