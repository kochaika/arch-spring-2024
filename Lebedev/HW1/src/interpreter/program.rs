use std::io;

use crate::interpreter::{Address, RawInstruction, Word};

pub struct ActualProgram {
    bytes: Vec<u8>,
}

impl ActualProgram {
    pub fn new(path: &String) -> io::Result<Self> {
        let bytes = std::fs::read(path)?;
        Ok(ActualProgram { bytes })
    }

    #[cfg(test)]
    pub fn from_bytes(bytes: Vec<u8>) -> Self {
        Self { bytes }
    }
}

pub trait Program {
    fn fetch_raw_instruction(&self, address: Address) -> RawInstruction;
    fn fetch_constant(&self, address: Address) -> Word;
}

impl Program for ActualProgram {
    fn fetch_raw_instruction(&self, address: Address) -> RawInstruction {
        let start = address.0 as usize;
        let end = start + RawInstruction::SIZE;
        RawInstruction::new(self.bytes[start..end].try_into().unwrap())
    }

    fn fetch_constant(&self, address: Address) -> Word {
        let address = address.0 as usize;
        let start = self.bytes.len() - address - std::mem::size_of::<Word>();
        let end = self.bytes.len() - address;
        Word::from_buffer(self.bytes[start..end].try_into().unwrap())
    }
}

#[cfg(test)]
pub mod tests {
    use strum::IntoEnumIterator;
    use crate::interpreter::{ActualProgram, Address, ALUInstructionKind, JumpInstructionKind, MemoryInstructionKind};
    use crate::interpreter::Instruction::{ALUInstruction, JumpInstruction, MemoryInstruction};
    use crate::interpreter::machine::Machine;
    use crate::interpreter::program::Program;
    use crate::interpreter::Reference::{AddressReference, StackReference};
    use crate::interpreter::registers::Registers;
    use crate::interpreter::state::State;

    #[test]
    fn test_alu() {
        for kind in ALUInstructionKind::iter() {
            let instruction = ALUInstruction {
                kind,
                res: Registers::alu_res(),
                lhs: Registers::alu_lhs(),
                rhs: Registers::alu_rhs(),
            };
            let buffer = instruction.create_buffer().to_vec();
            let program = ActualProgram::from_bytes(buffer);
            let raw_instruction = program.fetch_raw_instruction(Address::new(0));
            let state = Machine::<ActualProgram>::decode_instruction_open(&raw_instruction);
            let State::OperandsFetch(decoded) = state else {
                assert!(false);
                return
            };
            assert_eq!(instruction, decoded);
        }
    }

    #[test]
    fn test_jump() {
        for kind in JumpInstructionKind::iter() {
            let instruction = JumpInstruction {
                kind,
                reg: Registers::jump(),
                addr: Address(12345),
            };
            let buffer = instruction.create_buffer().to_vec();
            let program = ActualProgram::from_bytes(buffer);
            let raw_instruction = program.fetch_raw_instruction(Address::new(0));
            let state = Machine::<ActualProgram>::decode_instruction_open(&raw_instruction);
            let State::OperandsFetch(decoded) = state else {
                assert!(false);
                return
            };
            assert_eq!(instruction, decoded);
        }
    }

    #[test]
    fn test_memory() {
        for kind in MemoryInstructionKind::iter() {
            let instruction = MemoryInstruction {
                kind,
                reg: Registers::jump(),
                reference: if kind == MemoryInstructionKind::LoadConst {
                    AddressReference(Address::new(43))
                } else {
                    StackReference(44)
                },
            };
            let buffer = instruction.create_buffer().to_vec();
            let program = ActualProgram::from_bytes(buffer);
            let raw_instruction = program.fetch_raw_instruction(Address::new(0));
            let state = Machine::<ActualProgram>::decode_instruction_open(&raw_instruction);
            let State::OperandsFetch(decoded) = state else {
                assert!(false);
                return
            };
            assert_eq!(instruction, decoded);
        }
    }
}
