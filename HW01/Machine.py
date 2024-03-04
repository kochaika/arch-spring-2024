from ALU import ALU
from DataMemory import DataMemory
from FSM import FSM
from ProgramMemory import ProgramMemory
from RegisterFile import RegisterFile


class Machine:
    def __init__(self, num_registers, register_size, memory_size, word_size, states):
        self.alu = ALU()
        self.register_file = RegisterFile(num_registers, register_size)
        self.data_memory = DataMemory(memory_size, word_size)
        self.program_memory = ProgramMemory(memory_size, word_size)
        self.fsm = FSM(states)
        self.pc = 0

    def upload_program(self, program):
        for address, instruction in enumerate(program):
            self.program_memory.write_memory(address, instruction)

    def reset(self):
        self.pc = 0

    def clock(self):
        if self.pc < len(self.program_memory.memory):
            instruction = self.program_memory.read_memory(self.pc)
            self.fsm.set_state("FETCH")
            self.fsm.execute_state()
            self.pc += 1
            self.execute_instruction(instruction)

            return True
        else:
            return False

    def execute_instruction(self, instruction):
        opcode = (instruction >> 28) & 0b1111
        operands = instruction & 0b11111111
        source_register = (operands >> 5) & 0b111
        target_register = (operands >> 2) & 0b111
        destination_register = operands & 0b111

        operand1 = self.register_file.read_register(source_register)
        operand2 = self.register_file.read_register(target_register)

        result = self.alu.execute(operand1, operand2, opcode)

        self.register_file.write_register(destination_register, result)
