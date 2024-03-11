#ifndef CPU_H
#define CPU_H

typedef struct {
  void *instructions;
  int pc;
  int num_of_instructions;
} cpu;

cpu *cpu_init();
void reset(cpu *cpu_inst);
void cpu_execute(cpu *cpu);
void cpu_load_instructions(cpu *cpu, void *instr, int intr_num);

#endif // !CPU_H
