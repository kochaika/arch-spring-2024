use crate::interpreter::{Instruction, RawInstruction};

// "before ..."
#[derive(PartialEq, Debug)]
pub enum State {
    InstructionFetch,
    InstructionDecode(RawInstruction),
    OperandsFetch(Instruction),
    InstructionExecution(Instruction),
    ResultWrite(Instruction),
    Terminated,
}
