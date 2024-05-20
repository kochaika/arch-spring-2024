use std::ops::{Index, IndexMut};
use crate::interpreter::{Register, Word};

#[derive(Default)]
pub struct Registers {
    words: [Word; 32],
}

impl Index<Register> for Registers {
    type Output = Word;

    fn index(&self, index: Register) -> &Self::Output {
        &self.words[index.0 as usize]
    }
}

impl IndexMut<Register> for Registers {
    fn index_mut(&mut self, index: Register) -> &mut Self::Output {
        &mut self.words[index.0 as usize]
    }
}

macro_rules! registers {
    ($($name:ident -> $num:expr)*) => {
        impl Registers {
            $(
                pub const fn $name() -> Register {
                    Register($num)
                }
            )*
        }
    };
}

registers! {
    alu_lhs -> 1
    alu_rhs -> 2
    alu_res -> 1

    jump -> 1
    pc -> 2

    mem -> 1

    always_zero -> 0
}
