class ProgramMemory:
    def __init__(self, memory_size, word_size):
        self.memory_size = memory_size
        self.word_size = word_size
        self.memory = [0] * memory_size

    def read_memory(self, address):
        if 0 <= address < self.memory_size:
            return self.memory[address]
        else:
            raise ValueError("Invalid memory address")

    def write_memory(self, address, instruction):
        if 0 <= address < self.memory_size:
            self.memory[address] = instruction
        else:
            raise ValueError("Invalid memory address")
