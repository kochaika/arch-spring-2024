class ALU:
    def __init__(self):
        pass

    def execute(self, operand1, operand2, opcode):

        if opcode == 0b00:
            return self.add(operand1, operand2)
        elif opcode == 0b01:
            return self.subtract(operand1, operand2)
        elif opcode == 0b10:
            return self.bitwise_and(operand1, operand2)
        elif opcode == 0b11:
            return self.bitwise_or(operand1, operand2)
        else:
            raise ValueError("Unsupported operation code in ALU")

    def add(self, operand1, operand2):
        return operand1 + operand2

    def subtract(self, operand1, operand2):
        return operand1 - operand2

    def bitwise_and(self, operand1, operand2):
        return operand1 & operand2

    def bitwise_or(self, operand1, operand2):
        return operand1 | operand2

