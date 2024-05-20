use crate::interpreter::{ActualProgram, Address, ALUInstructionKind, Instruction, InstructionCode, JumpInstructionKind, MemoryInstructionKind, Printer, RawInstruction, StdPrinter, Word};
use crate::interpreter::alu::{ALU};
use crate::interpreter::memory::Memory;
use crate::interpreter::program::Program;
use crate::interpreter::registers::Registers;
use crate::interpreter::state::State;

#[derive(Copy, Clone)]
struct ProgramCounter(Address);

impl ProgramCounter {
    pub fn increment_once(self) -> Self {
        ProgramCounter(Address(self.0.0 + RawInstruction::SIZE as i32))
    }
}

impl Default for ProgramCounter {
    fn default() -> Self {
        ProgramCounter(Address::new(0))
    }
}

pub struct Machine<P: Program = ActualProgram, Prntr: Printer + Default = StdPrinter> {
    pc: ProgramCounter,
    program: P,
    memory: Memory,
    registers: Registers,
    state: State,
    alu: ALU<Prntr>,

    clock_cycles: i32, // debug only
}

impl <P: Program, Prntr: Printer + Default> Machine<P, Prntr> {
    pub fn new(program: P) -> Self {
        Machine {
            pc: ProgramCounter::default(),
            memory: Memory::default(),
            program,
            registers: Registers::default(),
            state: State::InstructionFetch,
            alu: ALU::default(),
            clock_cycles: 0,
        }
    }

    pub fn alu_printer(&self) -> &Prntr {
        self.alu.printer()
    }

    pub fn clock(&mut self) -> bool {
        self.state = match &self.state {
            State::InstructionFetch =>
                self.fetch_raw_instruction(),
            State::InstructionDecode(raw_instruction) =>
                Self::decode_instruction(raw_instruction),
            State::OperandsFetch(instruction) =>
                Self::fetch_operands(&self.pc, &mut self.registers, instruction),
            State::InstructionExecution(instruction) =>
                Self::execute_instruction(
                    &mut self.alu,
                    &mut self.memory,
                    &mut self.registers,
                    &self.program,
                    instruction,
                ),
            State::ResultWrite(instruction) =>
                Self::write_result(&mut self.pc, &mut self.registers, instruction),
            State::Terminated => panic!("Terminated twice!"),
        };
        self.clock_cycles += 1;
        !matches!(self.state, State::Terminated)
    }

    fn fetch_raw_instruction(&mut self) -> State {
        let raw_instruction = self.program.fetch_raw_instruction(self.pc.0);
        if raw_instruction.is_terminating() {
            State::Terminated
        } else {
            self.pc = self.pc.increment_once();
            State::InstructionDecode(raw_instruction)
        }
    }

    #[cfg(test)]
    pub fn decode_instruction_open(raw_instruction: &RawInstruction) -> State {
        Self::decode_instruction(raw_instruction)
    }

    fn decode_instruction(raw_instruction: &RawInstruction) -> State {
        let instruction_code = raw_instruction.instruction_code();
        if let Some(decoded) = Self::decode_instruction_inner(instruction_code, raw_instruction) {
            State::OperandsFetch(decoded)
        } else {
            eprintln!("Unexpected instruction code");
            State::Terminated
        }
    }

    fn decode_instruction_inner(instruction_code: InstructionCode, raw_instruction: &RawInstruction) -> Option<Instruction> {
        let alu = ALUInstructionKind::by_code(instruction_code).map(|kind| {
            Instruction::make_alu(kind, raw_instruction.remainder())
        });
        if let Some(alu) = alu { return alu.into() }

        let jump = JumpInstructionKind::by_code(instruction_code).map(|kind| {
            Instruction::make_jump(kind, raw_instruction.remainder())
        });
        if let Some(jump) = jump { return jump.into() }
        let mem = MemoryInstructionKind::by_code(instruction_code).map(|kind| {
            Instruction::make_memory(kind, raw_instruction.remainder())
        });
        if let Some(mem) = mem { return mem.into() }
        return None
    }

    fn fetch_operands(
        pc: &ProgramCounter,
        registers: &mut Registers,
        instruction: &Instruction,
    ) -> State {
        match instruction {
            Instruction::ALUInstruction { lhs, rhs, .. } => {
                registers[Registers::alu_lhs()] = registers[*lhs];
                registers[Registers::alu_rhs()] = registers[*rhs];
            }
            Instruction::JumpInstruction { kind, reg, .. } => {
                match kind {
                    JumpInstructionKind::JUMP => registers[Registers::jump()] = Word::new(1),
                    JumpInstructionKind::CJUMP => registers[Registers::jump()] = registers[*reg],
                }
                registers[Registers::pc()] = Word::new(pc.0.0);
            }
            Instruction::MemoryInstruction {
                kind, reg, ..
            } if matches!(kind, MemoryInstructionKind::Store) => {
                registers[Registers::mem()] = registers[*reg]
            }
            Instruction::MemoryInstruction { .. } => {}
        };
        State::InstructionExecution(*instruction)
    }

    fn execute_instruction(
        alu: &mut ALU<Prntr>,
        memory: &mut Memory,
        registers: &mut Registers,
        program: &P,
        instruction: &Instruction,
    ) -> State {
        match instruction {
            Instruction::ALUInstruction { kind, .. } => {
                alu.evaluate(registers, *kind);
                State::ResultWrite(*instruction)
            }
            Instruction::JumpInstruction { addr, reg, kind } => {
                let addr = match kind {
                    JumpInstructionKind::JUMP => {
                        Word::new(addr.0)
                    }
                    JumpInstructionKind::CJUMP => {
                        match registers[*reg].0 {
                            0 => Word::new(addr.0),
                            1 => registers[Registers::pc()],
                            _ => {
                                eprintln!("Bad word for jump!");
                                return State::Terminated;
                            }
                        }
                    }
                };
                registers[Registers::jump()] = addr;
                State::ResultWrite(*instruction)
            }
            Instruction::MemoryInstruction { kind, reference, .. } => {
                if let Some(state) = memory.evaluate(registers, program, *kind, *reference) {
                    return state
                } else {
                    State::ResultWrite(*instruction)
                }
            }
        }
    }

    fn write_result(
        pc: &mut ProgramCounter,
        registers: &mut Registers,
        instruction: &Instruction,
    ) -> State {
        match instruction {
            Instruction::ALUInstruction { res, .. } => {
                registers[*res] = registers[Registers::alu_res()];
            }
            Instruction::JumpInstruction { .. } => {
                pc.0 = Address(registers[Registers::jump()].0);
            }
            Instruction::MemoryInstruction {
                reg, kind, ..
            } if !matches!(kind, MemoryInstructionKind::Store) => {
                registers[*reg] = registers[Registers::mem()]
            }
            Instruction::MemoryInstruction { .. } => {}
        };
        State::InstructionFetch
    }
}
