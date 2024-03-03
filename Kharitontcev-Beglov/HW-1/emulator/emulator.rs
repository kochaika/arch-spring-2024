use crate::inst::Instr;

struct ALU;

impl ALU {
    pub fn new() -> Self {
        ALU {}
    }
}

struct Registers {
    data: [i32; 32],
}

impl Registers {
    pub fn new() -> Self {
        Self {
            data: [0; 32]
        }
    }
    
    pub fn reset(&mut self) {
        self.data.fill(0)
    }
}

struct Memory {
    data: [u32; 1024 * 1024],
    initial_memory: [u32; 1024 * 1024],
}

impl Memory {
    pub fn new(initial_memory: [u32; 1024 * 1024]) -> Self {
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
    pub fn new(commands: Vec<Instr>, initial_memory: [u32; 1024 * 1024]) -> Self {
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