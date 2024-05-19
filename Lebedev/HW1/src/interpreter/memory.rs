use std::collections::HashMap;

use crate::interpreter::{MemoryInstructionKind, Reference, Word};
use crate::interpreter::program::Program;
use crate::interpreter::registers::Registers;
use crate::interpreter::state::State;

#[derive(Default)]
pub struct Memory {
    data_cells: HashMap<usize, Word>,
}

impl Memory {
    pub fn evaluate(
        &mut self,
        registers: &mut Registers,
        program: &impl Program,
        kind: MemoryInstructionKind,
        reference: Reference,
    ) -> Option<State> {
        match (kind, reference) {
            (MemoryInstructionKind::Store, Reference::AddressReference(_)) => {
                eprintln!("Cannot write to constant");
                return Some(State::Terminated)
            }
            (MemoryInstructionKind::Store, Reference::StackReference(reference)) => {
                self.data_cells.insert(reference, registers[Registers::mem()]);
            }
            (MemoryInstructionKind::LoadStack, Reference::StackReference(reference)) =>
                if let Some(data) = self.data_cells.get(&reference) {
                    registers[Registers::mem()] = *data
                } else {
                    eprintln!("Not defined variable");
                    return Some(State::Terminated)
                }
            (MemoryInstructionKind::LoadConst, Reference::AddressReference(address)) =>
                registers[Registers::mem()] = program.fetch_constant(address),
            _ => {
                eprintln!("Bad load instruction");
                return Some(State::Terminated)
            }
        }
        None
    }
}

#[cfg(test)]
mod tests {
    use crate::interpreter::memory::Memory;
    use crate::interpreter::{Address, MemoryInstructionKind, RawInstruction, Reference, Word};
    use crate::interpreter::program::Program;
    use crate::interpreter::registers::Registers;
    use crate::interpreter::state::State;

    #[test]
    fn store_constant() {
        let result = Memory::default().evaluate(
            &mut Registers::default(),
            &MockProgram::new(vec![], vec![Word::new(1), Word::new(2), Word::new(3)]),
            MemoryInstructionKind::Store,
            Reference::AddressReference(Address::new(1)),
        );
        assert_eq!(result, Some(State::Terminated));
    }

    #[test]
    fn store_and_load() {
        let program = MockProgram::new(vec![], vec![Word::new(1), Word::new(2), Word::new(3)]);
        let mut memory = Memory::default();
        let mut registers = Registers::default();
        registers[Registers::mem()] = Word(42);
        let result = memory.evaluate(&mut registers, &program, MemoryInstructionKind::Store, Reference::StackReference(1));
        assert_eq!(result, None);
        registers[Registers::mem()] = Word(43);
        let result = memory.evaluate(&mut registers, &program, MemoryInstructionKind::LoadStack, Reference::StackReference(1));
        assert_eq!(result, None);
        assert_eq!(registers[Registers::mem()], Word(42));
    }

    pub struct MockProgram {
        raw_instructions: Vec<RawInstruction>,
        constants: Vec<Word>,
    }

    impl MockProgram {
        pub fn new(raw_instructions: Vec<RawInstruction>, constants: Vec<Word>) -> Self {
            MockProgram {
                raw_instructions,
                constants,
            }
        }
    }

    impl Program for MockProgram {
        fn fetch_raw_instruction(&self, address: Address) -> RawInstruction {
            self.raw_instructions[address.0 as usize]
        }

        fn fetch_constant(&self, address: Address) -> Word {
            self.constants[address.0 as usize]
        }
    }
}
