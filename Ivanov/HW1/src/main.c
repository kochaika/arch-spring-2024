#include "cpu.h"
#include "parse/parser.h"

int main() {
  int instr_cnt;
  cpu *machine = cpu_init();
  void *instructions = parse("./test_scripts/simple_test", &instr_cnt);
  cpu_load_instructions(machine, instructions, instr_cnt >> 1);
  cpu_execute(machine);
  return 0;
}
