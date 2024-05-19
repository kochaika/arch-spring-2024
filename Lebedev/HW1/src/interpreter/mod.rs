mod alu;
mod registers;
mod memory;
mod state;
mod program;
pub mod machine;

use strum_macros::EnumIter;
pub use program::ActualProgram;
pub use alu::printers::*;
pub use program::Program;

#[derive(Copy, Clone)]
struct InstructionCode(u8);

impl From<u8> for InstructionCode {
    fn from(value: u8) -> Self {
        Self(value)
    }
}

#[derive(Copy, Clone, Debug, Default, PartialEq)]
pub struct Word(i32);

impl Word {
    pub fn new(value: i32) -> Word {
        Word(value)
    }

    pub fn from_buffer(buffer: [u8; 4]) -> Word {
        Word(i32::from_ne_bytes(buffer))
    }

    pub fn create_buffer(&self) -> [u8; 4] {
        return self.0.to_ne_bytes();
    }
}

#[derive(Copy, Clone, Debug, PartialEq)]
pub struct Address(i32);

impl Address {
    pub const DUMMY: Address = Address(0);

    pub fn new(value: i32) -> Address {
        Address(value)
    }

    pub fn from_remainder(remainder: [u8; 4]) -> Address {
        Address(i32::from_ne_bytes(remainder))
    }

    pub fn create_remainder(&self) -> [u8; 4] {
        self.0.to_ne_bytes()
    }
}

#[derive(Copy, Clone, Debug, PartialEq)]
pub struct Register(u8);

impl Register {
    pub const fn new(value: u8) -> Register {
        Register(value)
    }
}

impl From<u8> for Register {
    fn from(value: u8) -> Self {
        Self(value)
    }
}

#[derive(Copy, Clone, Debug, PartialEq)]
pub enum Reference {
    AddressReference(Address),
    StackReference(usize)
}

impl Reference {
    pub fn stack_from_remainder(remainder: [u8; 4]) -> Reference {
        Reference::StackReference(i32::from_ne_bytes(remainder) as usize)
    }

    pub fn address_from_remainder(remainder: [u8; 4]) -> Reference {
        Reference::AddressReference(Address::from_remainder(remainder))
    }

    fn create_remainder(&self) -> [u8; 4] {
        match self {
            Reference::AddressReference(address) => address.create_remainder(),
            Reference::StackReference(stack) => (*stack as i32).to_ne_bytes(),
        }
    }
}

#[derive(Copy, Clone, Debug, PartialEq)]
pub struct RawInstruction {
    data: [u8; RawInstruction::SIZE]
}

pub type RawInstructionRemainderBuffer = [u8; RawInstruction::SIZE - 1];
pub type RawInstructionBuffer = [u8; RawInstruction::SIZE];

impl RawInstruction {
    pub const SIZE: usize = 6;

    pub fn new(data: RawInstructionBuffer) -> Self {
        Self { data }
    }

    pub fn is_terminating(&self) -> bool {
        self.data.iter().all(|b| *b == 0)
    }

    pub fn instruction_code(&self) -> InstructionCode {
        self.data[0].into()
    }

    pub fn remainder(&self) -> RawInstructionRemainderBuffer {
        self.data[1..RawInstruction::SIZE].try_into().unwrap()
    }
}

#[derive(Copy, Clone, Debug, PartialEq)]
pub enum Instruction {
    ALUInstruction {
        kind: ALUInstructionKind,
        res: Register,
        lhs: Register,
        rhs: Register,
    },

    JumpInstruction {
        kind: JumpInstructionKind,
        reg: Register,
        addr: Address,
    },

    MemoryInstruction {
        kind: MemoryInstructionKind,
        reg: Register,
        reference: Reference,
    },
}

impl Instruction {
    pub fn make_alu(
        kind: ALUInstructionKind,
        remainder: RawInstructionRemainderBuffer,
    ) -> Instruction {
        Instruction::ALUInstruction {
            kind,
            res: remainder[0].into(),
            lhs: remainder[1].into(),
            rhs: remainder[2].into(),
        }
    }

    pub fn make_jump(
        kind: JumpInstructionKind,
        remainder: RawInstructionRemainderBuffer,
    ) -> Instruction {
        Instruction::JumpInstruction {
            kind,
            reg: remainder[0].into(),
            addr: Address::from_remainder(remainder[1..5].try_into().unwrap()),
        }
    }

    pub fn make_memory(
        kind: MemoryInstructionKind,
        remainder: RawInstructionRemainderBuffer,
    ) -> Instruction {
        let reg = remainder[0].into();
        let remainder = remainder[1..5].try_into().unwrap();
        let reference = match kind {
            MemoryInstructionKind::LoadConst => Reference::address_from_remainder(remainder),
            MemoryInstructionKind::Store => Reference::stack_from_remainder(remainder),
            MemoryInstructionKind::LoadStack => Reference::stack_from_remainder(remainder),
        };
        Instruction::MemoryInstruction { kind, reg, reference, }
    }

    pub fn create_buffer(&self) -> RawInstructionBuffer {
        let mut buffer: RawInstructionBuffer = [0; RawInstruction::SIZE];
        self.fill_buffer(&mut buffer);
        buffer
    }

    fn fill_buffer(&self, buffer: &mut RawInstructionBuffer) {
        match self {
            Instruction::ALUInstruction { kind, res, lhs, rhs, } => {
                buffer[0] = kind.code().0;
                buffer[1] = res.0;
                buffer[2] = lhs.0;
                buffer[3] = rhs.0;
            }
            Instruction::JumpInstruction { kind, reg, addr } => {
                buffer[0] = kind.code().0;
                buffer[1] = reg.0;
                for (index, byte) in addr.create_remainder().iter().enumerate() {
                    buffer[2 + index] = *byte;
                }
            }
            Instruction::MemoryInstruction { kind, reg, reference } => {
                buffer[0] = kind.code().0;
                buffer[1] = reg.0;
                for (index, byte) in reference.create_remainder().iter().enumerate() {
                    buffer[2 + index] = *byte
                }
            }
        }
    }
}

macro_rules! instruction_kind {
    ($name:ident { $($num:expr => $item:ident)* }) => {
        #[derive(Copy, Clone, Debug, PartialEq, EnumIter)]
        pub enum $name {
            $($item,)*
        }

        impl $name {
            pub fn by_code(code: InstructionCode) -> Option<$name> {
                match code.0 {
                    $($num => { Some($name::$item) })*
                    _ => None
                }
            }

            pub fn code(&self) -> InstructionCode {
                match self {
                    $($name::$item => { InstructionCode($num) })*
                }
            }
        }
    };
}

instruction_kind! {
    ALUInstructionKind {
        0 => ADD
        1 => SUB
        2 => MUL
        3 => DIV
        4 => REM

        5 => SHL
        6 => SHR

        7 => EQ
        8 => LT

        9 => OR
        10 => AND
        11 => NOT

        17 => PRINT
    }
}

instruction_kind! {
    JumpInstructionKind {
        12 => JUMP
        13 => CJUMP // aka jump if zero
    }
}

instruction_kind! {
    MemoryInstructionKind {
        14 => Store
        15 => LoadStack
        16 => LoadConst
    }
}
