pub const MEMORY_SIZE: usize = 1024 * 1024;
const REGISTERS_SIZE: usize = 32;

struct ALU {
    zero_flag: bool,
}

impl ALU {
    pub fn new() -> Self {
        ALU { zero_flag: false }
    }
    pub fn reset(&mut self) {
        self.zero_flag = false;
    }
    pub fn perform_operation(&mut self, lhs: i32, rhs: i32, funct: u8) -> i32 {
        let result = match funct {
            32 => lhs.wrapping_add(rhs),
            34 => lhs.wrapping_sub(rhs),
            36 => lhs & rhs,
            37 => lhs | rhs,
            39 => !(lhs | rhs),
            42 => if lhs < rhs { 1 } else { 0 }
            _ => panic!("Invalid funct code={}", funct)
        };
        self.zero_flag = result == 0;
        result
    }
    pub fn get_zero_flag(&mut self) -> bool {
        self.zero_flag
    }
}

struct Registers {
    /// Each register is able to store a word
    data: [i32; REGISTERS_SIZE],
}

impl Registers {
    const STACK_POINTER_ID: usize = 31;
    const ZERO_REGISTER: usize = 0;
    pub fn new() -> Self {
        Self {
            data: [0; REGISTERS_SIZE]
        }
    }

    pub fn reset(&mut self) {
        self.data.fill(0)
    }
    pub fn get_value(&self, id: usize) -> i32 {
        self.data[id]
    }
    pub fn set_value(&mut self, id: usize, new_value: i32) {
        assert_ne!(0, id, "Can't alternate value for zeroth register");
        self.data[id] = new_value
    }
}

struct Memory {
    data: Vec<u8>,
    initial_memory: Vec<u8>,
}

impl Memory {
    pub fn new(initial_memory: Vec<u8>) -> Self {
        let mut res = Self {
            data: initial_memory.clone(),
            initial_memory,
        };
        res.initial_memory.resize(MEMORY_SIZE, 0);
        res.data.resize(MEMORY_SIZE, 0);
        res
    }
    pub fn reset(&mut self) {
        self.data.copy_from_slice(self.initial_memory.as_slice())
    }

    pub fn get_word_from_position(&self, position: usize) -> i32 {
        let slice = &self.data[position..(position + 4)];
        i32::from_be_bytes(slice.try_into().unwrap())
    }
    pub fn set_word_at_position(&mut self, position: usize, word: i32) {
        let bytes: &[u8; 4] = &word.to_be_bytes();
        self.data[position..(position + 4)].copy_from_slice(bytes)
    }
}

#[derive(Clone, Copy, Debug, Eq, PartialEq)]
enum FSMState {
    Fetch,
    Decode,
    RTypeExecute,
    RTypeALUWriteBack,
    JType,
    ITypeAddressCompute,
    ITypeMemoryRead,
    ITypeMemoryWrite,
    ITypeReadWriteback,
    Branch,
}

struct FSM {
    current_state: FSMState,
    opcode: u8,
    funct: u8,
}

#[derive(Copy, Clone, Debug)]
struct FSMDecision {
    pub iord: bool,
    pub mem_write: bool,
    pub ir_write: bool,
    pub pc_write: bool,
    pub branch: bool,
    pub pc_source: u8,
    pub alu_control: u8,
    pub alu_src_a_reg: bool,
    pub alu_source_b: u8,
    pub reg_write: bool,
    pub mem_to_reg: bool,
    pub reg_dst: bool,
    pub negate_zero: bool,
}

impl FSM {
    pub fn new() -> Self {
        Self { current_state: FSMState::Fetch, opcode: 0, funct: 0 }
    }
    pub fn set_instruction(&mut self, op: u8, funct: u8) {
        self.opcode = op;
        self.funct = funct;
    }
    pub fn reset(&mut self) {
        self.opcode = 0;
        self.funct = 0;
        self.current_state = FSMState::Fetch;
    }
    pub fn get_decision(&mut self) -> FSMDecision {
        match self.current_state {
            FSMState::Fetch => {
                self.current_state = FSMState::Decode;
                FSMDecision {
                    iord: false,
                    alu_src_a_reg: false,
                    alu_source_b: 1,
                    pc_source: 0,
                    mem_write: false,
                    branch: false,
                    ir_write: true,
                    pc_write: true,
                    alu_control: 32,
                    reg_write: false,
                    mem_to_reg: false,
                    reg_dst: false,
                    negate_zero: true,
                }
            }
            FSMState::Decode => {
                self.current_state = match self.opcode {
                    0 => FSMState::RTypeExecute,
                    2 => FSMState::JType,
                    4 => FSMState::Branch,
                    5 => FSMState::Branch,
                    34 => FSMState::ITypeAddressCompute,
                    43 => FSMState::ITypeAddressCompute,
                    _ => panic!("Invalid opcode {}!", self.opcode)
                };
                FSMDecision {
                    iord: false,
                    alu_src_a_reg: false,
                    alu_source_b: 3,
                    pc_source: 0,
                    mem_write: false,
                    branch: false,
                    ir_write: false,
                    pc_write: false,
                    alu_control: 32,
                    reg_write: false,
                    mem_to_reg: false,
                    negate_zero: true,
                    reg_dst: false,
                }
            }
            FSMState::ITypeAddressCompute => {
                self.current_state = if self.opcode == 34
                { FSMState::ITypeMemoryRead } else { FSMState::ITypeMemoryWrite };
                FSMDecision {
                    iord: false,
                    alu_src_a_reg: true,
                    alu_source_b: 2,
                    pc_source: 0,
                    mem_write: false,
                    branch: false,
                    ir_write: false,
                    pc_write: false,
                    alu_control: 32,
                    reg_write: false,
                    mem_to_reg: false,
                    reg_dst: false,
                    negate_zero: true,
                }
            }
            FSMState::ITypeMemoryRead => {
                self.current_state = FSMState::ITypeReadWriteback;
                FSMDecision {
                    iord: true,
                    alu_src_a_reg: true,
                    alu_source_b: 3,
                    pc_source: 0,
                    mem_write: false,
                    branch: false,
                    ir_write: false,
                    pc_write: false,
                    alu_control: 32,
                    reg_write: false,
                    mem_to_reg: false,
                    negate_zero: true,
                    reg_dst: false,
                }
            }
            FSMState::ITypeReadWriteback => {
                self.current_state = FSMState::Fetch;
                FSMDecision {
                    iord: false,
                    alu_src_a_reg: true,
                    alu_source_b: 3,
                    pc_source: 0,
                    mem_write: false,
                    branch: false,
                    ir_write: false,
                    pc_write: false,
                    alu_control: 32,
                    reg_write: true,
                    mem_to_reg: true,
                    negate_zero: true,
                    reg_dst: false,
                }
            }
            FSMState::ITypeMemoryWrite => {
                self.current_state = FSMState::Fetch;
                FSMDecision {
                    iord: true,
                    alu_src_a_reg: false,
                    alu_source_b: 0,
                    pc_source: 0,
                    mem_write: true,
                    branch: false,
                    ir_write: false,
                    pc_write: false,
                    alu_control: 32,
                    reg_write: false,
                    mem_to_reg: false,
                    negate_zero: true,
                    reg_dst: false,
                }
            }
            FSMState::RTypeExecute => {
                self.current_state = FSMState::RTypeALUWriteBack;
                FSMDecision {
                    iord: false,
                    alu_src_a_reg: true,
                    alu_source_b: 0,
                    pc_source: 0,
                    mem_write: false,
                    branch: false,
                    ir_write: false,
                    pc_write: false,
                    alu_control: self.funct,
                    reg_write: false,
                    mem_to_reg: false,
                    negate_zero: true,
                    reg_dst: false,
                }
            }
            FSMState::RTypeALUWriteBack => {
                self.current_state = FSMState::Fetch;
                FSMDecision {
                    iord: false,
                    alu_src_a_reg: true,
                    alu_source_b: 0,
                    pc_source: 0,
                    mem_write: false,
                    branch: false,
                    ir_write: false,
                    pc_write: false,
                    alu_control: self.funct,
                    reg_write: true,
                    mem_to_reg: false,
                    negate_zero: true,
                    reg_dst: true,
                }
            }
            FSMState::Branch => {
                self.current_state = FSMState::Fetch;
                FSMDecision {
                    iord: false,
                    alu_src_a_reg: true,
                    alu_source_b: 0,
                    pc_source: 1,
                    mem_write: false,
                    branch: true,
                    ir_write: false,
                    pc_write: false,
                    alu_control: 34,
                    reg_write: false,
                    mem_to_reg: false,
                    reg_dst: false,
                    negate_zero: self.opcode == 5,
                }
            }
            FSMState::JType => {
                self.current_state = FSMState::Fetch;
                FSMDecision {
                    iord: false,
                    alu_src_a_reg: false,
                    alu_source_b: 0,
                    pc_source: 2,
                    mem_write: false,
                    branch: false,
                    ir_write: false,
                    pc_write: true,
                    alu_control: 34,
                    reg_write: false,
                    mem_to_reg: false,
                    reg_dst: false,
                    negate_zero: false,
                }
            }
        }
    }
    pub fn is_print(&self) -> bool {
        self.opcode == 0 && self.funct == 0
    }
}

pub struct Emulator {
    commands: Vec<u8>,
    pc: usize,
    memory: Memory,
    alu: ALU,
    registers: Registers,
    fsm: FSM,
    current_instruction: u32,
    alu_output: i32,
    data: i32,
    operand_a: i32,
    operand_b: i32,
}

enum ReadMemoryFrom {
    Instruction(usize),
    Data(usize),
}

impl Emulator {
    pub fn new(commands: Vec<u8>, initial_memory: Vec<u8>) -> Self {
        Self {
            commands,
            pc: 0,
            memory: Memory::new(initial_memory),
            alu: ALU::new(),
            registers: Registers::new(),
            fsm: FSM::new(),
            current_instruction: 0,
            alu_output: 0,
            data: 0,
            operand_a: 0,
            operand_b: 0,
        }
    }

    pub fn reset(&mut self) {
        self.pc = 0;
        self.memory.reset();
        self.registers.reset();
        self.alu.reset();
        self.fsm.reset();
    }

    pub fn clock(&mut self) -> bool {
        // println!("Clock. PC={}", self.pc);
        // println!("FSMState: {:?}", self.fsm.current_state);
        // println!("Current instruction: {:#032b}. Opcode={}, funct={}", self.current_instruction, self.fsm.opcode, self.fsm.funct);
        let decision = self.fsm.get_decision();
        // println!("FSM Decision: {:?}", decision);
        // println!("operand_a={}, operand_b={}, alu_output={}, data={}", self.operand_a, self.operand_b, self.alu_output, self.data);

        let address = if !decision.iord {
            ReadMemoryFrom::Instruction(self.pc)
        } else {
            ReadMemoryFrom::Data(self.alu_output as usize)
        };

        if decision.mem_write {
            match address {
                ReadMemoryFrom::Instruction(_) => {}
                ReadMemoryFrom::Data(address) => {
                    // println!("mem[{}]={}", address, self.operand_b);
                    self.memory.set_word_at_position(address, self.operand_b)
                }
            }
        }

        let read = self.read(&address);

        if decision.ir_write {
            self.current_instruction = self.read(&address) as u32;
            self.fsm.set_instruction(
                ((self.current_instruction >> 26) & 0x3f) as u8,
                (self.current_instruction & 0x3f) as u8,
            );
            let rs = (self.current_instruction >> 21) & 0x1f;
            let rt = (self.current_instruction >> 16) & 0x1f;
            self.operand_a = self.registers.get_value(rs as usize);
            self.operand_b = self.registers.get_value(rt as usize);
            // println!("Current instruction (pc={}): {:#034b}. Opcode={}, funct={}, rs={}, rt={}", self.pc, self.current_instruction, self.fsm.opcode, self.fsm.funct, rs, rt);
            if self.fsm.is_print() {
                println!("{}", self.operand_a);
                // println!("Register: ${}={}", rs, self.operand_a);
                self.fsm.reset();
            }
        }

        if decision.reg_write {
            let data = if decision.mem_to_reg { self.data } else { self.alu_output };
            let res_reg = if decision.reg_dst { (self.current_instruction >> 11) & 0x1f } else { (self.current_instruction >> 16) & 0x1f };
            // println!("new reg #{}={}", res_reg, data);
            self.registers.set_value(res_reg as usize, data);
        }

        let alu_lhs: i32 = if decision.alu_src_a_reg { self.operand_a } else { self.pc as i32 };
        let alu_rhs: i32 = match decision.alu_source_b {
            0 => self.operand_b,
            1 => 4,
            2 => self.current_instruction as i32 & 0xff, // No sign extend
            3 => (self.current_instruction as i32 & 0xff) << 2,
            _ => panic!("Invalid alu_source_b: {}", decision.alu_source_b)
        };

        let result = self.alu.perform_operation(alu_lhs, alu_rhs, decision.alu_control);
        let pc_en = (decision.branch & (self.alu.get_zero_flag() ^ decision.negate_zero)) | (decision.pc_write);
        if pc_en {
            self.pc = match decision.pc_source {
                0 => result as usize,
                1 => self.alu_output as usize,
                2 => ((self.pc >> 28) << 28) | ((self.current_instruction as usize & 0x1ffffff) << 2),
                _ => panic!("Invalid PC source state: {}", decision.pc_source)
            };
            // println!("new pc={}, branch={}, zero={}, negate_zero={}", self.pc,decision.branch, self.alu.get_zero_flag(), decision.negate_zero);;
        }
        self.alu_output = result;
        self.data = read;
        // println!();
        self.pc >= self.commands.len() && self.fsm.current_state == FSMState::Fetch
    }

    fn read(&self, address: &ReadMemoryFrom) -> i32 {
        match *address {
            ReadMemoryFrom::Instruction(address) => {
                if address >= self.commands.len() {
                    assert_ne!(FSMState::Decode, self.fsm.current_state);
                    return 0 // Corner case: if we are reading this, that should be not instruction reading
                }
                let slice = &self.commands[address..(address + 4)];
                i32::from_be_bytes(slice.try_into().unwrap())
            }
            ReadMemoryFrom::Data(address) => {
                // print!("readind mem[{}]=", address);
                // println!("{}", self.memory.get_word_from_position(address));
                self.memory.get_word_from_position(address)
            }
        }
    }
}

#[cfg(test)]
mod tests;