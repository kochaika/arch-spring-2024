use crate::inst::Instr;

const MEMORY_SIZE: usize = 1024 * 1024;
const REGISTERS_SIZE: usize = 32;

struct ALU;

impl ALU {
    pub fn new() -> Self {
        ALU {}
    }
}

struct Registers {
    /// Each register is able to store a word
    data: [i32; REGISTERS_SIZE],
}

impl Registers {
    const STACK_POINTER_ID: usize = 31;
    pub fn new() -> Self {
        Self {
            data: [0; REGISTERS_SIZE]
        }
    }
    
    pub fn reset(&mut self) {
        self.data.fill(0)
    }
    pub fn sp(&self) -> i32 {
        self.data[Self::STACK_POINTER_ID]
    }
    pub fn set_sp(&mut self, new_value: i32) {
        self.data[Self::STACK_POINTER_ID] = new_value 
    }
}

struct Memory {
    data: [u8; MEMORY_SIZE],
    initial_memory: [u8; MEMORY_SIZE],
}

impl Memory {
    pub fn new(initial_memory: [u8; MEMORY_SIZE]) -> Self {
        Self {
            data: initial_memory,
            initial_memory
        }
    }
    pub fn reset(&mut self) {
        self.data.copy_from_slice(self.initial_memory.as_slice())
    }
}

pub struct Emulator {
    commands: Vec<Instr>,
    pc: usize,
    memory: Memory,
    alu: ALU,
    registers: Registers,
}

impl Emulator {
    pub fn new(commands: Vec<Instr>, initial_memory: [u8; MEMORY_SIZE]) -> Self {
        Self {
            commands,
            pc: 0,
            memory: Memory::new(initial_memory),
            alu: ALU::new(),
            registers: Registers::new(),
        }
    }

    pub fn reset(&mut self) {
        self.pc = 0;
        self.memory.reset();
        self.registers.reset();
    }
}