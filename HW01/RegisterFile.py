class RegisterFile:
    def __init__(self, num_registers, register_size):
        self.num_registers = num_registers
        self.register_size = register_size
        self.registers = [0] * num_registers

    def read_register(self, register_number):
        if 0 <= register_number < self.num_registers:
            return self.registers[register_number]
        else:
            raise ValueError("Invalid register number")

    def write_register(self, register_number, data):
        if 0 <= register_number < self.num_registers:
            self.registers[register_number] = data
        else:
            raise ValueError("Invalid register number")

